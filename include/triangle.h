#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"
#include "vertex.h"

class triangle : public hittable {
public:
    triangle(const vertex& v0, const vertex& v1, const vertex& v2, shared_ptr<material> mat)
        : v0(v0), v1(v1), v2(v2), mat_ptr(mat) {
        compute_normal();
        update_bounding_box();
    }

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
    virtual aabb bounding_box() const override { return bbox; }

private:
    vertex v0, v1, v2;
    shared_ptr<material> mat_ptr;
    aabb bbox;
    vec3 face_normal;

    void compute_normal() {
        // Compute face normal from cross product
        vec3 edge1 = v1.position - v0.position;
        vec3 edge2 = v2.position - v0.position;
        face_normal = unit_vector(cross(edge1, edge2));
    }

    void update_bounding_box() {
        // Compute AABB containing all three vertices
        double min_x = fmin(fmin(v0.position.x(), v1.position.x()), v2.position.x());
        double min_y = fmin(fmin(v0.position.y(), v1.position.y()), v2.position.y());
        double min_z = fmin(fmin(v0.position.z(), v1.position.z()), v2.position.z());

        double max_x = fmax(fmax(v0.position.x(), v1.position.x()), v2.position.x());
        double max_y = fmax(fmax(v0.position.y(), v1.position.y()), v2.position.y());
        double max_z = fmax(fmax(v0.position.z(), v1.position.z()), v2.position.z());

        // Add small padding to avoid edge cases
        const double epsilon = 0.0001;
        point3 pmin(min_x - epsilon, min_y - epsilon, min_z - epsilon);
        point3 pmax(max_x + epsilon, max_y + epsilon, max_z + epsilon);
        bbox = aabb(pmin, pmax);
    }
};

bool triangle::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    const double epsilon = 1e-8;

    vec3 edge1 = v1.position - v0.position;
    vec3 edge2 = v2.position - v0.position;
    vec3 ray_cross_edge2 = cross(r.direction(), edge2);
    double determinant = dot(edge1, ray_cross_edge2);

    // Check if ray is parallel to triangle
    if (fabs(determinant) < epsilon)
        return false;

    double inv_determinant = 1.0 / determinant;
    vec3 s = r.origin() - v0.position;
    double u = inv_determinant * dot(s, ray_cross_edge2);

    if (u < 0.0 || u > 1.0)
        return false;

    vec3 s_cross_edge1 = cross(s, edge1);
    double v = inv_determinant * dot(r.direction(), s_cross_edge1);

    if (v < 0.0 || u + v > 1.0)
        return false;

    double t = inv_determinant * dot(edge2, s_cross_edge1);

    if (t < t_min || t > t_max)
        return false;

    rec.t = t;
    rec.p = r.at(t);
    
    // Interpolate normal using barycentric coordinates
    double w = 1.0 - u - v;
    vec3 interpolated_normal = w * v0.normal + u * v1.normal + v * v2.normal;
    interpolated_normal = unit_vector(interpolated_normal);
    
    // Set normal facing the ray
    rec.set_face_normal(r, interpolated_normal);
    rec.mat_ptr = mat_ptr;

    return true;
}

#endif
