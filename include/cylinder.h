#ifndef CYLINDER_H
#define CYLINDER_H

#include "hittable.h"
#include "aabb.h"
#include "ray.h"

// Finite Y-aligned cylinder with radius r and caps at [y0,y1].
class cylinder : public hittable
{
public:
    cylinder(double y0_, double y1_, double radius_, std::shared_ptr<material> m, bool caps = true)
        : y0(y0_), y1(y1_), radius(radius_), mat_ptr(std::move(m)), closed(caps)
    {
        const double r = radius;
        bbox = aabb(point3(-r, y0, -r), point3(r, y1, r));
    }

    virtual bool hit(const ray &r, double t_min, double t_max, hit_record &rec) const override
    {
        // Side intersection with quadratic in XZ.
        const vec3 oc = r.origin();
        const vec3 d = r.direction();

        const double a = d.x() * d.x() + d.z() * d.z();
        if (std::fabs(a) < 1e-12)
        {
            // Ray parallel to cylinder axis.
            if (!closed)
                return false;
            return hit_caps(r, t_min, t_max, rec);
        }

        const double b = 2.0 * (oc.x() * d.x() + oc.z() * d.z());
        const double c = oc.x() * oc.x() + oc.z() * oc.z() - radius * radius;
        const double disc = b * b - 4 * a * c;
        if (disc < 0)
        {
            if (!closed)
                return false;
            return hit_caps(r, t_min, t_max, rec);
        }

        const double sqrt_disc = std::sqrt(disc);
        double root = (-b - sqrt_disc) / (2 * a);
        if (root < t_min || root > t_max)
        {
            root = (-b + sqrt_disc) / (2 * a);
            if (root < t_min || root > t_max)
            {
                if (!closed)
                    return false;
                hit_record cap_rec;
                if (hit_caps(r, t_min, t_max, cap_rec))
                {
                    rec = cap_rec;
                    return true;
                }
                return false;
            }
        }

        const double y = oc.y() + root * d.y();
        if (y < y0 || y > y1)
        {
            if (!closed)
                return false;
            hit_record cap_rec;
            if (hit_caps(r, t_min, t_max, cap_rec))
            {
                rec = cap_rec;
                return true;
            }
            return false;
        }

        rec.t = root;
        rec.p = r.at(root);
        vec3 outward_normal(rec.p.x() / radius, 0, rec.p.z() / radius);
        rec.set_face_normal(r, outward_normal);
        rec.mat_ptr = mat_ptr;
        return true;
    }

    virtual aabb bounding_box() const override { return bbox; }

private:
    double y0, y1;
    double radius;
    std::shared_ptr<material> mat_ptr;
    bool closed;
    aabb bbox;

    bool hit_caps(const ray &r, double t_min, double t_max, hit_record &rec) const
    {
        bool hit_any = false;
        double closest = t_max;
        hit_record best;

        for (int cap = 0; cap < 2; ++cap)
        {
            const double y_cap = (cap == 0) ? y0 : y1;
            const double t = (y_cap - r.origin().y()) / r.direction().y();
            if (t < t_min || t > closest)
                continue;
            const point3 p = r.at(t);
            const double x2z2 = p.x() * p.x() + p.z() * p.z();
            if (x2z2 > radius * radius)
                continue;

            hit_any = true;
            closest = t;
            best.t = t;
            best.p = p;
            vec3 outward_normal(0, (cap == 0) ? -1 : 1, 0);
            best.set_face_normal(r, outward_normal);
            best.mat_ptr = mat_ptr;
        }

        if (hit_any)
        {
            rec = best;
            return true;
        }
        return false;
    }
};

#endif
