# TODO: Extensive mesh support + shape primitives

## Step 1: Repo understanding
- [x] Inspect current core: hittable, BVH, mesh, triangle, loaders, scene_manager.
- [x] Identify gaps: analytic primitives missing; mesh representation limited; OBJ face parsing simplistic; mesh/bvh acceleration missing.

## Step 2: Add analytic primitives
- [ ] Add: axis-aligned box (aabox), rectangle (xy/xz/yz variants), plane, cylinder, cone.
- [ ] Ensure each primitive implements `hit()` and `bounding_box()`.
- [ ] Integrate into `scene_manager` with simple builder methods.

## Step 3: Upgrade mesh representation
- [ ] Replace `mesh` internals with indexed triangle mesh:
  - [ ] Store shared vertex buffer + shared index buffer.
  - [ ] Optionally build per-mesh triangle list for compatibility.
- [ ] Improve `triangle` to reference vertices by index (or add new `indexed_triangle`).
- [ ] Mesh bounding box computed from all vertices (not incremental per-triangle when possible).
- [ ] Optional: per-mesh BVH for internal acceleration.

## Step 4: Improve OBJ parsing
- [ ] Properly handle face vertex formats like `v`, `v/vt`, `v//vn`, `v/vt/vn`.
- [ ] Support negative indices.
- [ ] Triangulate polygons with >3 vertices.
- [ ] Build index buffer (unique vertex attribute triplets) instead of duplicating.

## Step 5: Wire into BVH
- [ ] Make sure `mesh` can return a single `bounding_box()` to the world BVH.
- [ ] If per-mesh BVH is implemented, `mesh::hit()` should delegate traversal to that BVH.

## Step 6: Update scene example
- [ ] Modify `src/main.cpp` to demonstrate new primitives and/or upgraded mesh.

## Step 7: Build & validate
- [ ] Build via CMake/MSVC/Ninja.
- [ ] Run and sanity-check intersections (spot artifacts).

