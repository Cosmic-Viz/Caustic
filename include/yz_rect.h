#ifndef YZ_RECT_H
#define YZ_RECT_H

#include "hittable.h"
#include "aabb.h"
#include "ray.h"
#include <cmath>

// Finite rectangle in the YZ plane at x=k.
class yz_rect : public hittable
{
public:
    yz_rect() = default;
    yz_rect(double y0_, double y1_, double z0_, double z1_, double k_, std::shared_ptr<material> m)
        : y0(y0_), y1(y1_), z0(z0_), z1(z1_), k(k_), mat_ptr(std::move(m)) {}

    virtual bool hit(const ray &r, double t_min, double t_max, hit_record &rec) const override
    {
        if (std::fabs(r.direction().x()) < 1e-12)
            return false;

        const double t = (k - r.origin().x()) / r.direction().x();
        if (t < t_min || t > t_max)
            return false;

        const double y = r.origin().y() + t * r.direction().y();
        const double z = r.origin().z() + t * r.direction().z();

        if (y < y0 || y > y1 || z < z0 || z > z1)
            return false;

        rec.t = t;
        rec.p = r.at(t);
        vec3 outward_normal(1, 0, 0);
        rec.set_face_normal(r, outward_normal);
        rec.mat_ptr = mat_ptr;
        return true;
    }

    virtual aabb bounding_box() const override
    {
        const double thickness = 0.0001;
        return aabb(point3(k - thickness, y0, z0), point3(k + thickness, y1, z1));
    }

private:
    double y0, y1, z0, z1, k;
    std::shared_ptr<material> mat_ptr;
};

#endif
