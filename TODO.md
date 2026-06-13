# TODO - Ray Tracing Performance Optimizations

## Step 1: Wire BVH into the render path
- [x] Update `src/main.cpp` to use BVH-backed world for `world.hit(...)`.
- [x] Ensure correctness (same output style / no crashes).




## Step 2: Optimize AABB traversal in BVH
- [ ] In `include/bvh.h`, reduce bounding_box() calls during hit.
- [ ] Precompute ray inverse direction once per `hit()` call.
- [ ] Reduce temporary allocations in traversal.

## Step 3: Improve BVH build heuristics
- [ ] Add optional SAH-based splitting (bins) in `include/bvh.h`.

## Step 4: Implement octree acceleration
- [ ] Add `include/octree.h` implementing `hittable`.
- [ ] Add octree build parameters: max depth / max leaf prims / epsilon stopping.
- [ ] Add octree traversal with near-first child ordering.

## Step 5: Hybrid approach (octree over meshes + BVH inside)
- [ ] If needed, add mesh-level octree and triangle/BVH internal.

## Step 6: Iterative traversal
- [ ] Convert recursive BVH traversal to explicit stack.

## Step 7: Ray-depth optimization (optional)
- [ ] Add Russian roulette termination.


