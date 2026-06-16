# Step 2 Plan: Analytic shape primitives (boxes, rectangles, planes, cylinders, cones)

## Info gathered
- Core architecture is `hittable` + `hit_record` + `aabb` + `bvh_node`.
- Current repo has only `sphere`, `triangle`, and `mesh` (mesh is hittable via internal `hittable_list` of triangles).
- `aabb` already supports fast slab intersection using cached ray inverse directions.

## Implemented primitives (targets)
1. `aabox` (axis-aligned box / cuboid) from `min/max`.
2. Rectangles: `xy_rect`, `xz_rect`, `yz_rect` (axis-aligned finite rectangles) with optional `k` constant plane.
3. `plane` (infinite plane) for convenience: bounded by a large AABB fallback OR implement as `hittable` without AABB (but BVH requires AABB), so we’ll implement `infinite_plane` by returning a very large AABB. (Deferred; not implemented in this step)
4. `cylinder` (finite along Y) + `cone` (finite along Y) aligned with Y axis, with caps optional.


## Constraints
- `hittable` interface requires `bounding_box()`.
- BVH uses `bounding_box()`; infinite primitives must have finite bounding boxes (we’ll implement as finite shapes).

## File-level edits/additions
- New headers in `include/`:
  - `include/aabox.h`
  - `include/xy_rect.h`, `include/xz_rect.h`, `include/yz_rect.h`
  - `include/box.h` (optional alias; could be the same as aabox)
  - `include/cylinder.h`
  - `include/cone.h`
- Update `include/scene_manager.h` to add builder methods:
  - `add_box(min,max,mat)`
  - `add_xy_rect(x0,x1,y0,y1,k,mat)` etc.
  - `add_cylinder(p0,p1,r,mat)` (Y-aligned for now)
  - `add_cone(p0,p1,r0,r1,mat)` (Y-aligned for now)
- Update `src/main.cpp` to add a small demo scene (primitive mix) so the change is visible.

## Followup
- Build & run.
- Render sanity check: ensure hits are correct and normals face correctly via `hit_record::set_face_normal`.

## <ask_followup_question>
Confirm you want the primitives limited to axis-aligned + Y-axis aligned (to keep math simpler and stable) for this step, and we will add arbitrary transforms later.
</ask_followup_question>

