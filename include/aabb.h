#ifndef AABB_H
#define AABB_H

#include "rtweekend.h"

class aabb {
public:
    aabb() {}
    aabb(const point3& a, const point3& b) : minimum(a), maximum(b) {}

    point3 min() const { return minimum; }
    point3 max() const { return maximum; }

    bool hit(const ray& r, double t_min, double t_max) const {
        for (int a = 0; a < 3; a++) {
            double invD = 1.0 / r.direction()[a];
            double t0 = (minimum[a] - r.origin()[a]) * invD;
            double t1 = (maximum[a] - r.origin()[a]) * invD;

            if (invD < 0.0)
                std::swap(t0, t1);

            t_min = fmax(t0, t_min);
            t_max = fmin(t1, t_max);

            if (t_max <= t_min)
                return false;
        }
        return true;
    }

    aabb surrounding_box(const aabb& box1, const aabb& box2) {
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

inline aabb surrounding_box(const aabb& box1, const aabb& box2) {
    point3 small(fmin(box1.min().x(), box2.min().x()),
                 fmin(box1.min().y(), box2.min().y()),
                 fmin(box1.min().z(), box2.min().z()));

    point3 big(fmax(box1.max().x(), box2.max().x()),
               fmax(box1.max().y(), box2.max().y()),
               fmax(box1.max().z(), box2.max().z()));

    return aabb(small, big);
}

#endif
