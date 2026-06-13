#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "hittable_list.h"
#include "mesh.h"
#include "obj_loader.h"
#include "stl_loader.h"
#include "ply_loader.h"
#include "material.h"
#include <string>
#include <unordered_map>
#include <memory>

class scene_manager {
public:
    scene_manager() : world(), materials() {}

    // Add a primitive sphere
    void add_sphere(point3 center, double radius, shared_ptr<material> mat) {
        auto sphere_obj = make_shared<sphere>(center, radius, mat);
        world.add(sphere_obj);
    }

    // Load OBJ mesh file
    shared_ptr<mesh> load_obj(const std::string& filepath, shared_ptr<material> mat) {
        auto loaded_mesh = obj_loader::load(filepath, mat);
        world.add(loaded_mesh);
        return loaded_mesh;
    }

    // Load STL mesh file
    shared_ptr<mesh> load_stl(const std::string& filepath, shared_ptr<material> mat) {
        auto loaded_mesh = stl_loader::load(filepath, mat);
        world.add(loaded_mesh);
        return loaded_mesh;
    }

    // Load PLY mesh file
    shared_ptr<mesh> load_ply(const std::string& filepath, shared_ptr<material> mat) {
        auto loaded_mesh = ply_loader::load(filepath, mat);
        world.add(loaded_mesh);
        return loaded_mesh;
    }

    // Load mesh file with automatic format detection
    shared_ptr<mesh> load_mesh(const std::string& filepath, shared_ptr<material> mat) {
        // Detect format by file extension
        size_t dot_pos = filepath.find_last_of('.');
        if (dot_pos == std::string::npos) {
            std::cerr << "Error: File has no extension" << std::endl;
            return make_shared<mesh>();
        }

        std::string extension = filepath.substr(dot_pos + 1);
        
        // Convert to lowercase
        for (auto& c : extension) c = std::tolower(c);

        if (extension == "obj") {
            return load_obj(filepath, mat);
        } else if (extension == "stl") {
            return load_stl(filepath, mat);
        } else if (extension == "ply") {
            return load_ply(filepath, mat);
        } else {
            std::cerr << "Error: Unsupported file format: " << extension << std::endl;
            return make_shared<mesh>();
        }
    }

    // Create material and store it
    shared_ptr<material> create_lambertian(const color& albedo) {
        auto mat = make_shared<lambertian>(albedo);
        materials.push_back(mat);
        return mat;
    }

    shared_ptr<material> create_metal(const color& albedo, double fuzz) {
        auto mat = make_shared<metal>(albedo, fuzz);
        materials.push_back(mat);
        return mat;
    }

    shared_ptr<material> create_dielectric(double ir) {
        auto mat = make_shared<dielectric>(ir);
        materials.push_back(mat);
        return mat;
    }

    // Get the world (hittable_list)
    const hittable_list& get_world() const { return world; }
    hittable_list& get_world() { return world; }

    // Get object count
    size_t object_count() const { return world.objects.size(); }

    // Clear the scene
    void clear() {
        world.objects.clear();
        materials.clear();
    }

private:
    hittable_list world;
    std::vector<shared_ptr<material>> materials;
};

#endif
