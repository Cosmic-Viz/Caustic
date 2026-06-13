#ifndef STL_LOADER_H
#define STL_LOADER_H

#include "mesh.h"
#include "vertex.h"
#include "material.h"
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <cstring>

class stl_loader {
public:
    static shared_ptr<mesh> load_ascii(const std::string& filepath, shared_ptr<material> default_material) {
        auto loaded_mesh = make_shared<mesh>();
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open STL file: " << filepath << std::endl;
            return loaded_mesh;
        }

        std::string line;
        std::vector<vec3> normals;
        
        while (std::getline(file, line)) {
            if (line.find("facet normal") != std::string::npos) {
                // Parse normal
                vec3 normal;
                if (std::sscanf(line.c_str(), "facet normal %lf %lf %lf", 
                    &normal.x(), &normal.y(), &normal.z()) == 3) {
                    normals.push_back(unit_vector(normal));
                }

                // Skip "outer loop"
                std::getline(file, line);

                // Read three vertices
                point3 v[3];
                for (int i = 0; i < 3; i++) {
                    std::getline(file, line);
                    if (std::sscanf(line.c_str(), " vertex %lf %lf %lf", 
                        &v[i].x(), &v[i].y(), &v[i].z()) != 3) {
                        std::cerr << "Error parsing vertex" << std::endl;
                        continue;
                    }
                }

                // Create triangle
                vertex v0(v[0], normals.back());
                vertex v1(v[1], normals.back());
                vertex v2(v[2], normals.back());
                loaded_mesh->add_triangle(v0, v1, v2, default_material);

                // Skip "endloop" and "endfacet"
                std::getline(file, line);
                std::getline(file, line);
            }
        }

        file.close();
        std::cout << "Loaded ASCII STL: " << filepath << " with " 
                  << loaded_mesh->triangle_count() << " triangles" << std::endl;
        return loaded_mesh;
    }

    static shared_ptr<mesh> load_binary(const std::string& filepath, shared_ptr<material> default_material) {
        auto loaded_mesh = make_shared<mesh>();
        
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open STL file: " << filepath << std::endl;
            return loaded_mesh;
        }

        // Skip 80-byte header
        char header[80];
        file.read(header, 80);

        // Read number of triangles
        uint32_t num_triangles;
        file.read(reinterpret_cast<char*>(&num_triangles), sizeof(uint32_t));

        for (uint32_t i = 0; i < num_triangles; i++) {
            // Read normal (3 floats)
            float normal[3];
            file.read(reinterpret_cast<char*>(normal), 12);
            vec3 face_normal(normal[0], normal[1], normal[2]);
            face_normal = unit_vector(face_normal);

            // Read vertices (3 vertices × 3 floats each)
            float vertex_data[9];
            file.read(reinterpret_cast<char*>(vertex_data), 36);

            point3 v[3];
            for (int j = 0; j < 3; j++) {
                v[j] = point3(vertex_data[j * 3], vertex_data[j * 3 + 1], vertex_data[j * 3 + 2]);
            }

            // Skip attribute byte count
            uint16_t attribute_byte_count;
            file.read(reinterpret_cast<char*>(&attribute_byte_count), 2);

            // Create triangle
            vertex v0(v[0], face_normal);
            vertex v1(v[1], face_normal);
            vertex v2(v[2], face_normal);
            loaded_mesh->add_triangle(v0, v1, v2, default_material);
        }

        file.close();
        std::cout << "Loaded binary STL: " << filepath << " with " 
                  << loaded_mesh->triangle_count() << " triangles" << std::endl;
        return loaded_mesh;
    }

    static shared_ptr<mesh> load(const std::string& filepath, shared_ptr<material> default_material) {
        // Check if file is binary or ASCII by looking at the header
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open STL file: " << filepath << std::endl;
            return make_shared<mesh>();
        }

        char header[5];
        file.read(header, 5);
        file.close();

        // ASCII STL files start with "solid"
        if (std::strncmp(header, "solid", 5) == 0) {
            return load_ascii(filepath, default_material);
        } else {
            return load_binary(filepath, default_material);
        }
    }
};

#endif
