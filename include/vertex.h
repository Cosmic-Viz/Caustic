#ifndef VERTEX_H
#define VERTEX_H

#include "rtweekend.h"

struct vertex {
    point3 position;    // 3D position
    vec3 normal;        // Surface normal
    vec3 texcoord;      // UV coordinates (z unused, keeping vec3 for consistency)
    vec3 tangent;       // Tangent for normal mapping (future use)
    vec3 bitangent;     // Bitangent for normal mapping (future use)

    vertex() : position(0, 0, 0), normal(0, 1, 0), texcoord(0, 0, 0),
               tangent(1, 0, 0), bitangent(0, 0, 1) {}

    vertex(point3 pos, vec3 norm = vec3(0, 1, 0), vec3 uv = vec3(0, 0, 0))
        : position(pos), normal(norm), texcoord(uv),
          tangent(1, 0, 0), bitangent(0, 0, 1) {}

    vertex(point3 pos, vec3 norm, vec3 uv, vec3 tan, vec3 bitan)
        : position(pos), normal(norm), texcoord(uv),
          tangent(tan), bitangent(bitan) {}
};

#endif
