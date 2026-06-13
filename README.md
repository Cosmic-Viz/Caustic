# Caustic - Ray Tracer

A custom-built ray tracer based on "Ray Tracing in One Weekend" by Peter Shirley, with additional features and enhancements.

## Features

### Core Features
- **Vector Math**: Full 3D vector operations (vec3 class)
- **Ray Tracing**: Recursive ray tracing with configurable bounce depth
- **Spheres**: Geometric sphere primitives
- **Materials**:
  - Lambertian (diffuse)
  - Metal (reflective with fuzzy reflections)
  - Dielectric (transparent/glass)
- **Camera**: Realistic camera with adjustable aperture for depth of field
- **Sampling**: Multi-sample anti-aliasing
- **Acceleration**: OpenMP parallel rendering

## Building

Requirements:
- C++17 or higher
- CMake 3.10 or higher
- OpenMP support

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## Running

```bash
./caustic > output.ppm
```

The output is in PPM format. Convert to PNG or other formats using ImageMagick or similar tools:

```bash
convert output.ppm output.png
```

## Configuration

Edit `src/main.cpp` to adjust:
- Image resolution (aspect_ratio, image_width)
- Sampling quality (samples_per_pixel)
- Ray bounce depth (max_depth)
- Camera settings (lookfrom, lookat, aperture, focus distance)
- Scene composition (add/remove spheres, modify materials)

## Future Enhancements

- [ ] Additional shape primitives (planes, triangles, meshes)
- [ ] Texture mapping
- [ ] Normal mapping
- [ ] Bump mapping
- [ ] More material types (emissive, subsurface scattering)
- [ ] BVH acceleration structure
- [ ] Configurable scene files
- [ ] GPU acceleration (CUDA/OptiX)

## References

- "Ray Tracing in One Weekend" by Peter Shirley
- https://raytracing.github.io/

## License

MIT License - Feel free to use and modify this code
