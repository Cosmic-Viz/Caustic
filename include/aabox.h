#ifndef AABOX_H
#define AABOX_H

#include "hittable.h"
#include "aabb.h"
#include "ray.h"

// Axis-aligned box defined by min/max corners.
class aabox : public hittable
{
public:
    aabox() = default;

    aabox(const point3 &min_c, const point3 &max_c, std::shared_ptr<material> m)
        : box(min_c, max_c), mat_ptr(std::move(m))
    {
        bbox = box;
    }

    virtual bool hit(const ray &r, double t_min, double t_max, hit_record &rec) const override
    {
        const double t_near = bbox.entry_time(r, t_min, t_max);
        if (t_near == infinity)
            return false;

        rec.t = t_near;
        rec.p = r.at(rec.t);

        const auto &mn = bbox.min();
        const auto &mx = bbox.max();

        const double eps = 1e-6;
        vec3 outward_normal(0, 0, 0);

        if (std::fabs(rec.p.x() - mn.x()) < eps)
            outward_normal = vec3(-1, 0, 0);
        else if (std::fabs(rec.p.x() - mx.x()) < eps)
            outward_normal = vec3(1, 0, 0);
        else if (std::fabs(rec.p.y() - mn.y()) < eps)
            outward_normal = vec3(0, -1, 0);
        else if (std::fabs(rec.p.y() - mx.y()) < eps)
            outward_normal = vec3(0, 1, 0);
        else if (std::fabs(rec.p.z() - mn.z()) < eps)
            outward_normal = vec3(0, 0, -1);
        else
            outward_normal = vec3(0, 0, 1);

        rec.set_face_normal(r, outward_normal);
        rec.mat_ptr = mat_ptr;
        return true;
    }

    virtual aabb bounding_box() const override { return bbox; }

private:
    aabb box{};
    aabb bbox{};
    std::shared_ptr<material> mat_ptr;
};

#endif
