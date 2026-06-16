#ifndef XZ_RECT_H
#define XZ_RECT_H

#include "hittable.h"
#include "aabb.h"
#include "ray.h"
#include <cmath>

// Finite rectangle in the XZ plane at y=k.
class xz_rect : public hittable
{
public:
    xz_rect() = default;
    xz_rect(double x0_, double x1_, double z0_, double z1_, double k_, std::shared_ptr<material> m)
        : x0(x0_), x1(x1_), z0(z0_), z1(z1_), k(k_), mat_ptr(std::move(m)) {}

    virtual bool hit(const ray &r, double t_min, double t_max, hit_record &rec) const override
    {
        if (std::fabs(r.direction().y()) < 1e-12)
            return false;

        const double t = (k - r.origin().y()) / r.direction().y();
        if (t < t_min || t > t_max)
            return false;

        const double x = r.origin().x() + t * r.direction().x();
        const double z = r.origin().z() + t * r.direction().z();

        if (x < x0 || x > x1 || z < z0 || z > z1)
            return false;

        rec.t = t;
        rec.p = r.at(t);
        vec3 outward_normal(0, 1, 0);
        rec.set_face_normal(r, outward_normal);
        rec.mat_ptr = mat_ptr;
        return true;
    }

    virtual aabb bounding_box() const override
    {
        const double thickness = 0.0001;
        return aabb(point3(x0, k - thickness, z0), point3(x1, k + thickness, z1));
    }

private:
    double x0, x1, z0, z1, k;
    std::shared_ptr<material> mat_ptr;
};

#endif
