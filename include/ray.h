#ifndef RAY_H
#define RAY_H

#include "vec3.h"
#include "rtweekend.h"

class ray

{
public:
    ray() : orig(), dir(), inv_dir(), sign{false, false, false} {}

    ray(const point3 &origin, const vec3 &direction)
        : orig(origin), dir(direction)
    {
        // Cache inverse direction for fast slab intersection.
        // Note: direction components are typically non-zero, but handle zeros safely.
        for (int a = 0; a < 3; ++a)
        {
            const double d = dir[a];
            const double inv = (d != 0.0) ? (1.0 / d) : ((d < 0.0) ? -infinity : infinity);
            inv_dir[a] = inv;
            sign[a] = (inv < 0.0);
        }
    }

    const point3 &origin() const { return orig; }
    const vec3 &direction() const { return dir; }

    const vec3 &invDirection() const { return inv_dir; }
    const bool *raySign() const { return sign; }

    point3 at(double t) const
    {
        return orig + t * dir;
    }

private:
    point3 orig;
    vec3 dir;

    // Cached for AABB intersection.
    vec3 inv_dir;
    bool sign[3];
};

#endif
