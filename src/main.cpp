#include "rtweekend.h"
#include "camera.h"
#include "hittable_list.h"
#include "bvh.h"
#include "material.h"

#include "sphere.h"
#include "aabox.h"
#include "xy_rect.h"
#include "xz_rect.h"
#include "yz_rect.h"
#include "cylinder.h"
#include "cone.h"
#include <iostream>
#include <omp.h>

color ray_color(const ray &r, const hittable &world, int depth)
{
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0, 0, 0);

    if (world.hit(r, 0.001, infinity, rec))
    {
        ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth - 1);
        return color(0, 0, 0);
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

int main()
{
    // Image
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 1200;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 500;
    const int max_depth = 50;

    // World (build a temporary list, then wrap it in BVH)
    hittable_list world;

    auto material_ground = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left = make_shared<dielectric>(1.5);
    auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);
    auto material_pink = make_shared<lambertian>(color(0.9, 0.2, 0.6));
    auto material_green = make_shared<metal>(color(0.2, 0.8, 0.3), 0.05);

    // Ground plane approximation via huge sphere
    world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));

    // Keep a couple of spheres
    world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
    world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
    world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

    // New primitives demo
    // AABB box (axis-aligned cuboid)
    world.add(std::make_shared<aabox>(point3(-0.6, 0.1, -3.0), point3(0.6, 1.2, -2.0), material_pink));

    // Rectangles (finite) at different planes
    world.add(std::make_shared<xy_rect>(-1.0, 1.0, -0.5, 0.7, -2.5, material_green));
    world.add(std::make_shared<xz_rect>(-1.0, 1.0, -2.0, 0.5, -0.2, material_center));
    // Cylinder aligned with Y-axis, radius 0.4, y in [0,1.4]
    world.add(std::make_shared<cylinder>(0.0, 1.4, 0.4, material_right, true));
    // Cone aligned with Y-axis, from radius 0.0 at y=0 to 0.45 at y=1.3
    world.add(std::make_shared<cone>(0.0, 1.3, 0.0, 0.45, material_left));

    // Wrap scene objects in BVH for faster ray traversal
    hittable_list bvh_world;
    bvh_world.add(std::make_shared<bvh_node>(world.objects, 0, world.objects.size()));

    const hittable &render_world = bvh_world;

    // Camera

    point3 lookfrom(13, 2, 3);
    point3 lookat(0, 0, 0);
    vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;

    camera cam(lookfrom, lookat, vup, 20.0, aspect_ratio, aperture, dist_to_focus);

    // Render
    std::cout << "P3\n"
              << image_width << ' ' << image_height << "\n255\n";

#pragma omp parallel for collapse(2) schedule(dynamic)
    for (int j = image_height - 1; j >= 0; --j)
    {
        for (int i = 0; i < image_width; ++i)
        {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s)
            {
                auto u = (i + random_double()) / (image_width - 1);
                auto v = (j + random_double()) / (image_height - 1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, render_world, max_depth);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
    }
    std::cerr << "\nDone.\n";

    return 0;
}
