#ifndef XY_RECT_H
#define XY_RECT_H

#include "hittable.h"
#include "aabb.h"
#include "ray.h"
#include <cmath>

// Finite rectangle in the XY plane at z=k.
class xy_rect : public hittable
{
public:
    xy_rect() = default;
    xy_rect(double x0_, double x1_, double y0_, double y1_, double k_, std::shared_ptr<material> m)
        : x0(x0_), x1(x1_), y0(y0_), y1(y1_), k(k_), mat_ptr(std::move(m)) {}

    virtual bool hit(const ray &r, double t_min, double t_max, hit_record &rec) const override
    {
        if (std::fabs(r.direction().z()) < 1e-12)
            return false;

        const double t = (k - r.origin().z()) / r.direction().z();
        if (t < t_min || t > t_max)
            return false;

        const double x = r.origin().x() + t * r.direction().x();
        const double y = r.origin().y() + t * r.direction().y();

        if (x < x0 || x > x1 || y < y0 || y > y1)
            return false;

        rec.t = t;
        rec.p = r.at(t);
        vec3 outward_normal(0, 0, 1);
        rec.set_face_normal(r, outward_normal);
        rec.mat_ptr = mat_ptr;
        return true;
    }

    virtual aabb bounding_box() const override
    {
        const double thickness = 0.0001;
        return aabb(point3(x0, y0, k - thickness), point3(x1, y1, k + thickness));
    }

private:
    double x0, x1, y0, y1, k;
    std::shared_ptr<material> mat_ptr;
};

#endif
