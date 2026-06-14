#ifndef CONE_H
#define CONE_H

#include "hittable.h"
#include "aabb.h"
#include "ray.h"

// Finite Y-aligned right circular cone with apex at y0 (radius r0) and base at y1 (radius r1).
// If r0==0 this is a typical cone.
class cone : public hittable
{
public:
    cone(double y0_, double y1_, double r0_, double r1_, std::shared_ptr<material> m)
        : y0(y0_), y1(y1_), r0(r0_), r1(r1_), mat_ptr(std::move(m))
    {
        const double rmax = (r0 > r1) ? r0 : r1;
        bbox = aabb(point3(-rmax, y0, -rmax), point3(rmax, y1, rmax));
    }

    virtual bool hit(const ray &r, double t_min, double t_max, hit_record &rec) const override
    {
        // Solve intersection with implicit cone equation.
        // For Y-axis cone, radius as function of y: rt(y) = r0 + k*(y-y0), k=(r1-r0)/(y1-y0)
        const double h = y1 - y0;
        if (std::fabs(h) < 1e-12)
            return false;

        const double k = (r1 - r0) / h;

        // We use equation: x^2 + z^2 = (r0 + k*(y - y0))^2
        // Plug ray: x=ox+tdx, y=oy+tdy, z=oz+tdz -> quadratic in t.
        const double ox = r.origin().x();
        const double oy = r.origin().y();
        const double oz = r.origin().z();
        const double dx = r.direction().x();
        const double dy = r.direction().y();
        const double dz = r.direction().z();

        const double A = dx * dx + dz * dz - (k * dy) * (k * dy);
        const double B = 2.0 * (ox * dx + oz * dz) - 2.0 * (r0 + k * (oy - y0)) * (k * dy);
        const double C = (ox * ox + oz * oz) - (r0 + k * (oy - y0)) * (r0 + k * (oy - y0));

        const double disc = B * B - 4 * A * C;
        if (disc < 0)
            return false;
        const double sqrt_disc = std::sqrt(disc);

        double t = (-B - sqrt_disc) / (2 * A);
        if (t < t_min || t > t_max)
        {
            t = (-B + sqrt_disc) / (2 * A);
            if (t < t_min || t > t_max)
                return false;
        }

        const double y = oy + t * dy;
        if (y < y0 || y > y1)
            return false;

        rec.t = t;
        rec.p = r.at(t);

        // Compute normal approximately: for cone, gradient of F(x,y,z)=x^2+z^2 - rt(y)^2
        const double rt = r0 + k * (y - y0);
        // dF/dx=2x, dF/dz=2z, dF/dy = -2*rt*k
        vec3 outward_normal(rec.p.x(), -(rt * k), rec.p.z());
        outward_normal = unit_vector(outward_normal);

        rec.set_face_normal(r, outward_normal);
        rec.mat_ptr = mat_ptr;
        return true;
    }

    virtual aabb bounding_box() const override { return bbox; }

private:
    double y0, y1;
    double r0, r1;
    std::shared_ptr<material> mat_ptr;
    aabb bbox;
};

#endif
