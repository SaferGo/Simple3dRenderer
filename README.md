# Simple 3D Renderer

## About
The aim of this project was to learn how a very simple modern renderer works so that I could start building my own. This is by no means finished, and I don't plan to complete it. I am leaving it as a reference for myself, primarily for the rasterization part which can sometimes be confusing. I followed (partially) the amazing [tutorial](https://github.com/ssloy/tinyrenderer) by ssloy.

## Render Sample
![output](https://user-images.githubusercontent.com/19228971/209626350-28fb827e-675e-4ffe-8a5e-a39bc08e43eb.jpg)


## Features
**Renderer**
<ul>
  <li> Vertex and Fragment shaders </li>
  <li> Rasterizer </li>
  <li> Normal Mapping </li>
</ul>

## Third party libraries
Here's the list of the libraries included in the project:

* [Aseprite TGA Library](https://github.com/aseprite/tga): Library to read/write Truevision TGA/TARGA files.
* [GLM](https://github.com/g-truc/glm): Mathematics library for graphics software.
* [tinyobjloader](https://github.com/tinyobjloader/tinyobjloader): Tiny but powerful single file wavefront obj loader.

## Dependencies
- cmake >= 3.9.1
- gcc >= 9.4.0

## Building on Linux
```diff
$ git clone --recurse-submodules https://github.com/SaferGo/Simple3dRenderer.git
$ cd Simple3dRenderer/
$ bash run.sh
// img generated in /output
```

## Documentation
- Fundamental of Computer Graphics (Fourth edition)
by Steve Marschner and Peter Shirley.
(Chapter 8: The Graphics Pipeline)
- https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/rasterization-stage
- https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/visibility-problem-depth-buffer-depth-interpolation
