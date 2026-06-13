#ifndef MESH_H
#define MESH_H

#include "hittable_list.h"
#include "triangle.h"
#include "vertex.h"
#include <vector>
#include <string>

class mesh : public hittable {
public:
    mesh() : triangles(), bbox() {}

    void add_triangle(const vertex& v0, const vertex& v1, const vertex& v2, 
                      shared_ptr<material> mat) {
        auto tri = make_shared<triangle>(v0, v1, v2, mat);
        triangles.add(tri);
        
        // Update bounding box
        if (triangles.objects.size() == 1) {
            bbox = tri->bounding_box();
        } else {
            bbox = surrounding_box(bbox, tri->bounding_box());
        }
    }

    void add_vertices(const std::vector<vertex>& vertices,
                      const std::vector<unsigned int>& indices,
                      shared_ptr<material> mat) {
        for (size_t i = 0; i < indices.size(); i += 3) {
            if (indices[i] < vertices.size() &&
                indices[i + 1] < vertices.size() &&
                indices[i + 2] < vertices.size()) {
                add_triangle(vertices[indices[i]],
                             vertices[indices[i + 1]],
                             vertices[indices[i + 2]], mat);
            }
        }
    }

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
        return triangles.hit(r, t_min, t_max, rec);
    }

    virtual aabb bounding_box() const override { return bbox; }

    size_t triangle_count() const { return triangles.objects.size(); }

private:
    hittable_list triangles;
    aabb bbox;
};

#endif
