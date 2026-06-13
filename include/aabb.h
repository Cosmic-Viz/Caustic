#ifndef AABB_H
#define AABB_H

#include "rtweekend.h"

class aabb
{
public:
    aabb() {}
    aabb(const point3 &a, const point3 &b) : minimum(a), maximum(b) {}

    const point3 &min() const { return minimum; }
    const point3 &max() const { return maximum; }

    // Returns the parametric entry time (t_near) for the ray against this AABB.
    // If there is no intersection within [t_min, t_max], returns infinity.
    double entry_time(const ray &r, double t_min, double t_max) const
    {
        const vec3 &invD = r.invDirection();
        const bool *sign = r.raySign();

        double t0 = t_min;
        double t1 = t_max;

        for (int a = 0; a < 3; a++)
        {
            const double slab_t0 = ((sign[a] ? maximum[a] : minimum[a]) - r.origin()[a]) * invD[a];
            const double slab_t1 = ((sign[a] ? minimum[a] : maximum[a]) - r.origin()[a]) * invD[a];

            t0 = fmax(t0, slab_t0);
            t1 = fmin(t1, slab_t1);

            if (t1 <= t0)
                return infinity;
        }

        return t0;
    }

    bool hit(const ray &r, double t_min, double t_max) const
    {
        return entry_time(r, t_min, t_max) != infinity;
    }

    aabb surrounding_box(const aabb &box1, const aabb &box2)
    {
        point3 small(fmin(box1.min().x(), box2.min().x()),
                     fmin(box1.min().y(), box2.min().y()),
                     fmin(box1.min().z(), box2.min().z()));

        point3 big(fmax(box1.max().x(), box2.max().x()),
                   fmax(box1.max().y(), box2.max().y()),
                   fmax(box1.max().z(), box2.max().z()));

        return aabb(small, big);
    }

private:
    point3 minimum;
    point3 maximum;
};

inline aabb surrounding_box(const aabb &box1, const aabb &box2)
{
    point3 small(fmin(box1.min().x(), box2.min().x()),
                 fmin(box1.min().y(), box2.min().y()),
                 fmin(box1.min().z(), box2.min().z()));

    point3 big(fmax(box1.max().x(), box2.max().x()),
               fmax(box1.max().y(), box2.max().y()),
               fmax(box1.max().z(), box2.max().z()));

    return aabb(small, big);
}

#endif
