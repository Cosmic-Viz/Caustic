#ifndef PLY_LOADER_H
#define PLY_LOADER_H

#include "mesh.h"
#include "vertex.h"
#include "material.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include <map>

class ply_loader {
private:
    struct ply_header {
        int vertex_count = 0;
        int face_count = 0;
        bool has_normals = false;
        bool has_texcoords = false;
        std::vector<std::string> vertex_properties;
    };

public:
    static shared_ptr<mesh> load(const std::string& filepath, shared_ptr<material> default_material) {
        auto loaded_mesh = make_shared<mesh>();
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open PLY file: " << filepath << std::endl;
            return loaded_mesh;
        }

        ply_header header;
        std::string line;

        // Parse header
        std::getline(file, line); // "ply"
        if (line.find("ply") == std::string::npos) {
            std::cerr << "Error: Not a valid PLY file" << std::endl;
            return loaded_mesh;
        }

        bool in_header = true;
        while (in_header && std::getline(file, line)) {
            if (line.empty()) continue;

            std::istringstream iss(line);
            std::string token;
            iss >> token;

            if (token == "element") {
                std::string type;
                int count;
                iss >> type >> count;
                if (type == "vertex") {
                    header.vertex_count = count;
                } else if (type == "face") {
                    header.face_count = count;
                }
            }
            else if (token == "property") {
                std::string type, name;
                iss >> type >> name;
                header.vertex_properties.push_back(name);
                if (name == "nx" || name == "ny" || name == "nz") {
                    header.has_normals = true;
                }
                if (name == "s" || name == "t" || name == "u" || name == "v") {
                    header.has_texcoords = true;
                }
            }
            else if (token == "end_header") {
                in_header = false;
            }
        }

        // Parse vertices
        std::vector<point3> positions;
        std::vector<vec3> normals;
        std::vector<vec3> texcoords;

        for (int i = 0; i < header.vertex_count; i++) {
            std::getline(file, line);
            std::istringstream iss(line);
            
            double x, y, z;
            if (!(iss >> x >> y >> z)) break;
            
            positions.push_back(point3(x, y, z));

            // Parse optional normal
            if (header.has_normals) {
                double nx, ny, nz;
                if (iss >> nx >> ny >> nz) {
                    normals.push_back(unit_vector(vec3(nx, ny, nz)));
                }
            }

            // Parse optional texcoords
            if (header.has_texcoords) {
                double u, v;
                if (iss >> u >> v) {
                    texcoords.push_back(vec3(u, v, 0.0));
                }
            }
        }

        // Parse faces
        for (int i = 0; i < header.face_count; i++) {
            std::getline(file, line);
            std::istringstream iss(line);
            
            int vertex_count;
            if (!(iss >> vertex_count)) break;

            std::vector<int> face_indices(vertex_count);
            for (int j = 0; j < vertex_count; j++) {
                if (!(iss >> face_indices[j])) break;
            }

            // Triangulate if necessary
            for (int j = 1; j < vertex_count - 1; j++) {
                int idx0 = face_indices[0];
                int idx1 = face_indices[j];
                int idx2 = face_indices[j + 1];

                if (idx0 < (int)positions.size() && idx1 < (int)positions.size() && idx2 < (int)positions.size()) {
                    vertex v0(positions[idx0]);
                    vertex v1(positions[idx1]);
                    vertex v2(positions[idx2]);

                    if (!normals.empty()) {
                        v0.normal = normals[idx0];
                        v1.normal = normals[idx1];
                        v2.normal = normals[idx2];
                    }

                    if (!texcoords.empty()) {
                        v0.texcoord = texcoords[idx0];
                        v1.texcoord = texcoords[idx1];
                        v2.texcoord = texcoords[idx2];
                    }

                    loaded_mesh->add_triangle(v0, v1, v2, default_material);
                }
            }
        }

        file.close();
        std::cout << "Loaded PLY: " << filepath << std::endl;
        std::cout << "  Vertices: " << positions.size() << std::endl;
        std::cout << "  Normals: " << normals.size() << std::endl;
        std::cout << "  Triangles: " << loaded_mesh->triangle_count() << std::endl;

        return loaded_mesh;
    }
};

#endif
