#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "hittable_list.h"
#include "mesh.h"
#include "obj_loader.h"
#include "stl_loader.h"
#include "ply_loader.h"
#include "material.h"

#include "sphere.h"
#include "aabox.h"
#include "xy_rect.h"
#include "xz_rect.h"
#include "yz_rect.h"
#include "cylinder.h"
#include "cone.h"

#include <string>
#include <unordered_map>
#include <memory>

class scene_manager
{
public:
    scene_manager() : world(), materials() {}

    // Add a primitive sphere
    void add_sphere(point3 center, double radius, shared_ptr<material> mat)
    {
        auto sphere_obj = make_shared<sphere>(center, radius, mat);
        world.add(sphere_obj);
    }

    void add_box(const point3 &min_c, const point3 &max_c, shared_ptr<material> mat)
    {
        world.add(std::make_shared<aabox>(min_c, max_c, mat));
    }

    void add_xy_rect(double x0, double x1, double y0, double y1, double k, shared_ptr<material> mat)
    {
        world.add(std::make_shared<xy_rect>(x0, x1, y0, y1, k, mat));
    }

    void add_xz_rect(double x0, double x1, double z0, double z1, double k, shared_ptr<material> mat)
    {
        world.add(std::make_shared<xz_rect>(x0, x1, z0, z1, k, mat));
    }

    void add_yz_rect(double y0, double y1, double z0, double z1, double k, shared_ptr<material> mat)
    {
        world.add(std::make_shared<yz_rect>(y0, y1, z0, z1, k, mat));
    }

    void add_cylinder(double y0, double y1, double radius, shared_ptr<material> mat, bool caps = true)
    {
        world.add(std::make_shared<cylinder>(y0, y1, radius, mat, caps));
    }

    void add_cone(double y0, double y1, double r0, double r1, shared_ptr<material> mat)
    {
        world.add(std::make_shared<cone>(y0, y1, r0, r1, mat));
    }

    // Load OBJ mesh file
    shared_ptr<mesh> load_obj(const std::string &filepath, shared_ptr<material> mat)
    {
        auto loaded_mesh = obj_loader::load(filepath, mat);
        world.add(loaded_mesh);
        return loaded_mesh;
    }

    // Load STL mesh file
    shared_ptr<mesh> load_stl(const std::string &filepath, shared_ptr<material> mat)
    {
        auto loaded_mesh = stl_loader::load(filepath, mat);
        world.add(loaded_mesh);
        return loaded_mesh;
    }

    // Load PLY mesh file
    shared_ptr<mesh> load_ply(const std::string &filepath, shared_ptr<material> mat)
    {
        auto loaded_mesh = ply_loader::load(filepath, mat);
        world.add(loaded_mesh);
        return loaded_mesh;
    }

    // Load mesh file with automatic format detection
    shared_ptr<mesh> load_mesh(const std::string &filepath, shared_ptr<material> mat)
    {
        // Detect format by file extension
        size_t dot_pos = filepath.find_last_of('.');
        if (dot_pos == std::string::npos)
        {
            std::cerr << "Error: File has no extension" << std::endl;
            return make_shared<mesh>();
        }

        std::string extension = filepath.substr(dot_pos + 1);

        // Convert to lowercase
        for (auto &c : extension)
            c = std::tolower(c);

        if (extension == "obj")
        {
            return load_obj(filepath, mat);
        }
        else if (extension == "stl")
        {
            return load_stl(filepath, mat);
        }
        else if (extension == "ply")
        {
            return load_ply(filepath, mat);
        }
        else
        {
            std::cerr << "Error: Unsupported file format: " << extension << std::endl;
            return make_shared<mesh>();
        }
    }

    // Create material and store it
    shared_ptr<material> create_lambertian(const color &albedo)
    {
        auto mat = make_shared<lambertian>(albedo);
        materials.push_back(mat);
        return mat;
    }

    shared_ptr<material> create_metal(const color &albedo, double fuzz)
    {
        auto mat = make_shared<metal>(albedo, fuzz);
        materials.push_back(mat);
        return mat;
    }

    shared_ptr<material> create_dielectric(double ir)
    {
        auto mat = make_shared<dielectric>(ir);
        materials.push_back(mat);
        return mat;
    }

    // Get the world (hittable_list)
    const hittable_list &get_world() const { return world; }
    hittable_list &get_world() { return world; }

    // Get object count
    size_t object_count() const { return world.objects.size(); }

    // Clear the scene
    void clear()
    {
        world.objects.clear();
        materials.clear();
    }

private:
    hittable_list world;
    std::vector<shared_ptr<material>> materials;
};

#endif
