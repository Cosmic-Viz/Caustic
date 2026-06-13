#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include "mesh.h"
#include "vertex.h"
#include "material.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>

class obj_loader {
public:
    static shared_ptr<mesh> load(const std::string& filepath, shared_ptr<material> default_material) {
        auto loaded_mesh = make_shared<mesh>();
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open OBJ file: " << filepath << std::endl;
            return loaded_mesh;
        }

        std::vector<point3> positions;
        std::vector<vec3> normals;
        std::vector<vec3> texcoords;
        std::vector<unsigned int> indices;

        std::string line;
        int line_num = 0;

        while (std::getline(file, line)) {
            line_num++;
            
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#')
                continue;

            std::istringstream iss(line);
            std::string token;
            iss >> token;

            if (token == "v") {
                // Vertex position
                double x, y, z;
                if (iss >> x >> y >> z) {
                    positions.push_back(point3(x, y, z));
                } else {
                    std::cerr << "Warning: Invalid vertex at line " << line_num << std::endl;
                }
            }
            else if (token == "vn") {
                // Vertex normal
                double x, y, z;
                if (iss >> x >> y >> z) {
                    normals.push_back(unit_vector(vec3(x, y, z)));
                } else {
                    std::cerr << "Warning: Invalid normal at line " << line_num << std::endl;
                }
            }
            else if (token == "vt") {
                // Texture coordinate
                double u, v;
                if (iss >> u >> v) {
                    texcoords.push_back(vec3(u, v, 0.0));
                } else {
                    std::cerr << "Warning: Invalid texcoord at line " << line_num << std::endl;
                }
            }
            else if (token == "f") {
                // Face (supports triangles and quads)
                std::vector<unsigned int> face_indices;
                std::string vertex_str;

                while (iss >> vertex_str) {
                    unsigned int v_idx = 0, vt_idx = 0, vn_idx = 0;
                    
                    // Parse vertex/texcoord/normal indices
                    size_t slash1 = vertex_str.find('/');
                    size_t slash2 = vertex_str.find('/', slash1 + 1);

                    if (slash1 != std::string::npos) {
                        v_idx = std::stoi(vertex_str.substr(0, slash1)) - 1;
                        
                        if (slash2 != std::string::npos) {
                            std::string middle = vertex_str.substr(slash1 + 1, slash2 - slash1 - 1);
                            if (!middle.empty()) {
                                vt_idx = std::stoi(middle) - 1;
                            }
                            vn_idx = std::stoi(vertex_str.substr(slash2 + 1)) - 1;
                        } else {
                            std::string middle = vertex_str.substr(slash1 + 1);
                            if (!middle.empty()) {
                                vt_idx = std::stoi(middle) - 1;
                            }
                        }
                    } else {
                        v_idx = std::stoi(vertex_str) - 1;
                    }

                    face_indices.push_back(v_idx);
                }

                // Triangulate if necessary (quads -> two triangles)
                for (size_t i = 1; i < face_indices.size() - 1; i++) {
                    unsigned int idx0 = face_indices[0];
                    unsigned int idx1 = face_indices[i];
                    unsigned int idx2 = face_indices[i + 1];

                    if (idx0 < positions.size() && idx1 < positions.size() && idx2 < positions.size()) {
                        vertex v0(positions[idx0]);
                        vertex v1(positions[idx1]);
                        vertex v2(positions[idx2]);

                        // Set normals if available
                        if (!normals.empty()) {
                            v0.normal = normals[std::min((size_t)idx0, normals.size() - 1)];
                            v1.normal = normals[std::min((size_t)idx1, normals.size() - 1)];
                            v2.normal = normals[std::min((size_t)idx2, normals.size() - 1)];
                        }

                        // Set texcoords if available
                        if (!texcoords.empty()) {
                            v0.texcoord = texcoords[std::min((size_t)idx0, texcoords.size() - 1)];
                            v1.texcoord = texcoords[std::min((size_t)idx1, texcoords.size() - 1)];
                            v2.texcoord = texcoords[std::min((size_t)idx2, texcoords.size() - 1)];
                        }

                        loaded_mesh->add_triangle(v0, v1, v2, default_material);
                    }
                }
            }
        }

        file.close();

        std::cout << "Loaded OBJ: " << filepath << std::endl;
        std::cout << "  Vertices: " << positions.size() << std::endl;
        std::cout << "  Normals: " << normals.size() << std::endl;
        std::cout << "  Texcoords: " << texcoords.size() << std::endl;
        std::cout << "  Triangles: " << loaded_mesh->triangle_count() << std::endl;

        return loaded_mesh;
    }
};

#endif
