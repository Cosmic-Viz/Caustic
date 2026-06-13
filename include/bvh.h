#ifndef BVH_H
#define BVH_H

#include "hittable.h"
#include "aabb.h"

#include <algorithm>
#include <cstddef>
#include <memory>
#include <vector>

#include "rtweekend.h"

class bvh_node : public hittable
{
public:
    bvh_node() = default;

    bvh_node(std::vector<std::shared_ptr<hittable>> &objects, std::size_t start, std::size_t end)
    {
        build(objects, start, end);
    }

    virtual bool hit(const ray &r, double t_min, double t_max, hit_record &rec) const override
    {
        if (!bbox.hit(r, t_min, t_max))
            return false;

        // Nearest-first traversal: test the child whose AABB is entered first.
        const aabb left_box = left ? left->bounding_box() : aabb{};
        const aabb right_box = right ? right->bounding_box() : aabb{};

        auto ray_entry_t = [&r](const aabb &box, double t_min_local, double t_max_local) -> double
        {
            // Compute entry t using slab intersection (duplicated to avoid changing aabb API).
            double t0 = t_min_local;
            double t1 = t_max_local;

            for (int a = 0; a < 3; a++)
            {
                const double invD = 1.0 / r.direction()[a];
                double slab_t0 = (box.min()[a] - r.origin()[a]) * invD;
                double slab_t1 = (box.max()[a] - r.origin()[a]) * invD;

                if (invD < 0.0)
                    std::swap(slab_t0, slab_t1);

                t0 = fmax(t0, slab_t0);
                t1 = fmin(t1, slab_t1);

                if (t1 <= t0)
                    return infinity; // no intersection in range
            }

            return t0;
        };

        const bool has_left = (left != nullptr);
        const bool has_right = (right != nullptr);

        if (has_left && !has_right)
            return left->hit(r, t_min, t_max, rec);
        if (!has_left && has_right)
            return right->hit(r, t_min, t_max, rec);

        const double t_left_entry = ray_entry_t(left_box, t_min, t_max);
        const double t_right_entry = ray_entry_t(right_box, t_min, t_max);

        // If both are valid, visit nearer first.
        const bool visit_left_first = t_left_entry <= t_right_entry;

        hit_record near_rec;
        bool hit_near = false;
        hit_record far_rec;
        bool hit_far = false;

        if (visit_left_first)
        {
            hit_near = left->hit(r, t_min, t_max, near_rec);
            hit_far = right->hit(r, t_min, hit_near ? near_rec.t : t_max, far_rec);
        }
        else
        {
            hit_near = right->hit(r, t_min, t_max, near_rec);
            hit_far = left->hit(r, t_min, hit_near ? near_rec.t : t_max, far_rec);
        }

        if (hit_far)
        {
            rec = far_rec;
            return true;
        }
        if (hit_near)
        {
            rec = near_rec;
            return true;
        }

        return false;
    }

    virtual aabb bounding_box() const override { return bbox; }

private:
    std::shared_ptr<hittable> left;
    std::shared_ptr<hittable> right;
    aabb bbox;

    void build(std::vector<std::shared_ptr<hittable>> &objects, std::size_t start, std::size_t end)
    {
        const std::size_t object_span = end - start;
        if (object_span == 1)
        {
            left = right = objects[start];
            bbox = objects[start]->bounding_box();
            return;
        }

        // Precompute centroids once for each object to avoid repeated bounding_box() calls.
        struct centroid_entry
        {
            std::shared_ptr<hittable> obj;
            aabb box;
            point3 centroid;
        };

        std::vector<centroid_entry> entries;
        entries.reserve(object_span);

        for (std::size_t i = start; i < end; ++i)
        {
            const auto &obj = objects[i];
            const aabb box = obj->bounding_box();
            const point3 c((box.min().x() + box.max().x()) * 0.5,
                           (box.min().y() + box.max().y()) * 0.5,
                           (box.min().z() + box.max().z()) * 0.5);
            entries.push_back({obj, box, c});
        }

        // Choose split axis based on largest centroid spread.
        vec3 centroid_min(infinity, infinity, infinity);
        vec3 centroid_max(-infinity, -infinity, -infinity);
        for (const auto &e : entries)
        {
            centroid_min[0] = fmin(centroid_min[0], e.centroid[0]);
            centroid_min[1] = fmin(centroid_min[1], e.centroid[1]);
            centroid_min[2] = fmin(centroid_min[2], e.centroid[2]);

            centroid_max[0] = fmax(centroid_max[0], e.centroid[0]);
            centroid_max[1] = fmax(centroid_max[1], e.centroid[1]);
            centroid_max[2] = fmax(centroid_max[2], e.centroid[2]);
        }

        vec3 centroid_extent = centroid_max - centroid_min;
        int axis = 0;
        if (centroid_extent[1] > centroid_extent[0])
            axis = 1;
        if (centroid_extent[2] > centroid_extent[axis])
            axis = 2;

        std::sort(entries.begin(), entries.end(), [axis](const centroid_entry &a, const centroid_entry &b)
                  { return a.centroid[axis] < b.centroid[axis]; });

        // Write back sorted objects to keep existing recursion signature.
        for (std::size_t i = 0; i < entries.size(); ++i)
            objects[start + i] = entries[i].obj;

        const std::size_t mid = start + object_span / 2;
        left = std::make_shared<bvh_node>(objects, start, mid);
        right = std::make_shared<bvh_node>(objects, mid, end);

        bbox = surrounding_box(left->bounding_box(), right->bounding_box());
    }
};

#endif
