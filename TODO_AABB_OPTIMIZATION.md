# TODO - AABB Optimization

## Step 1: Validate current hotspots
- [x] Inspect `include/aabb.h` slab intersection implementation
- [x] Inspect `include/bvh.h` traversal (notably duplicated entry-t slab math)
- [x] Inspect `include/ray.h` / `include/vec3.h` to see if ray can be extended/cached

## Step 2: Implement best-performance ray-cached slab data
- [x] Extend `ray` with cached inverse direction and sign bits
- [x] Update `ray` constructors to compute cached values
- [x] Update `aabb::hit()` to use cached inverse direction and sign


## Step 3: Remove BVH duplicate slab math

- [x] Reduced BVH duplicated slab work by using ray cached invDir/sign
- [x] Refactor `bvh_node::hit()` to reuse AABB entry-t helper
- [x] Ensure correctness of near/far traversal ordering (smoke render succeeded)




## Step 4: Build and test
- [x] Rebuild Release
- [x] Run `caustic.exe` smoke test (generated `out.ppm`)


## Step 5: Optional micro-opts
- [x] Make `aabb::hit` use refs/const references and avoid temporaries (done via `const&` and ref usage)
- [x] Reduce `min()/max()` / access overhead (done via cached `invDirection()`/`raySign()` and reusing them inside the slab loop)


