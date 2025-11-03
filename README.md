# C++ Software Rasterizer

This is a complete 3D rendering pipeline built from scratch in C++. It is written in standard C++ with no external graphics libraries (like OpenGL or DirectX). Its sole purpose is to take 3D model data and render a fully shaded 2D image, pixel by pixel.

The final renderer is capable of loading complex `.obj` models, applying textures, and performing dynamic Blinn-Phong lighting with a Z-buffer for correct depth.



## Core Features

* **Model Loading:** Parses `.obj` files, loading vertices, texture coordinates (UVs), and normals.
* **3D-to-2D Projection:** Implements a full Model-View-Projection (MVP) matrix pipeline for 3D transformation.
* **Triangle Rasterization:** Uses barycentric coordinates to efficiently fill triangles.
* **Depth Buffering:** A complete Z-buffer ensures that objects correctly occlude each other.
* **Texturing:** Loads `.tga` files and applies them using perspective-correct interpolation.
* **Shading:** A flexible shader-based architecture (using an `IShader` interface) implements the Blinn-Phong reflection model for realistic lighting.
* **Optimizations:** Includes back-face culling to avoid drawing unnecessary triangles.

---

## The Build Process: From Pixels to Phong

This project was built in 5 distinct phases, with each phase adding a major component of the graphics pipeline.

### Phase 1: The Foundation (2D Canvas & Math)

* **Goal:** Create a 2D image and draw lines on it.
* **Key Components:**
    * `Color.h`: A simple `struct` to hold RGBA pixel data.
    * `Image.h`/`.cpp`: A class that manages a 1D `std::vector` as a 2D pixel buffer and includes a function to write the buffer to a `.tga` (Targa) file.
    * `Vec.h`: Core math library for `Vec2i`, `Vec2f`, and `Vec3f`, including operator overloading for vector arithmetic.
    * **Bresenham's Line Algorithm:** Implemented to draw 2D lines efficiently using only integer math.
* **Result:** A program that could save a `.tga` file with 2D lines drawn on it.

### Phase 2: The 3D Wireframe Pipeline

* **Goal:** Project 3D points onto the 2D screen to draw a wireframe.
* **Key Components:**
    * `Mat4f.h`: A 4x4 matrix class with all necessary 3D transformation functions:
        * `identity()`, `translation()`, `scaling()`, `rotation_x/y/z()`
        * `lookAt()` (View Matrix)
        * `perspective()` (Projection Matrix)
    * `Model.h`: A simple class to hold a list of `Vec3f` vertices and integer-pair "edges."
    * **The Projection Pipeline:** A `project()` function was created to perform the full **Model-View-Projection (MVP)** transformation, **perspective divide** (dividing by `w`), and **viewport transform** (mapping from NDC to screen coordinates).
* **Result:** A program that rendered a 3D wireframe cube, which could be rotated and viewed from different angles.
    

### Phase 3: Solid Objects & Depth

* **Goal:** Fill in the wireframe triangles and make sure they draw in the correct order.
* **Key Components:**
    * **Z-Buffer:** A `std::vector<float>` was added to the `Image` class, parallel to the color buffer, to store the depth of every pixel.
    * **Barycentric Coordinates:** The `drawLine` function was replaced with `drawTriangle`. This algorithm finds the 2D bounding box of a triangle and iterates over each pixel, using barycentric coordinates (`u, v, w`) to determine if the pixel is inside.
    * **Depth Test:** Before drawing a pixel, its interpolated depth was compared against the Z-buffer. The pixel was only drawn if it was closer (had a smaller `z` value) than what was already in the buffer.
    * **Back-Face Culling:** An optimization to skip drawing any triangle "facing away" from the camera, calculated by taking the dot product of the triangle's normal and the view direction.
* **Result:** A solid, flat-shaded 3D cube with correct occlusion (front faces hide back faces).
    

### Phase 4: Parsing & Texturing

* **Goal:** Load a "real" 3D model (`.obj`) and apply a texture to it.
* **Key Components:**
    * **`.obj` Parser:** The `Model` class was completely rewritten to include a parser that reads `.obj` files, correctly parsing `v` (vertex), `vt` (texture UV), `vn` (normal), and `f` (face index) lines.
    * `Texture.h`/`.cpp`: A new class built to read `.tga` texture files into a buffer.
    * `sample(u, v)`: A function to sample the color from the texture buffer given UV coordinates.
    * **Perspective-Correct Interpolation:** The `drawTriangle` function was upgraded. Instead of just interpolating `u` and `v`, it interpolates `u/w` and `v/w`, and `1/w`. At each pixel, it divides `(u/w) / (1/w)` to find the correct UV coordinate, preventing texture "warping."
    * **Debugging:** This phase involved significant debugging of coordinate systems (e.g., flipping the model's `v.y` on load, reversing culling logic) to match the file data to the renderer's world space.
* **Result:** A program that could load and render the `african_head.obj` model with its full texture.
    

### Phase 5: Blinn-Phong Shading

* **Goal:** Add realistic lighting to give the model volume and highlights.
* **Key Components:**
    * **Shader Refactor:** The "monolithic" `drawTriangle` function was broken apart.
    * `IShader.h`: An abstract interface class was created to define a "Vertex Shader" (`vertex()`) and a "Fragment Shader" (`fragment()`).
    * `PhongShader.h`/`.cpp`: A concrete implementation of `IShader`.
        * **Vertex Shader:** Runs once per vertex. It transforms vertices into clip-space and passes "varying" data (like interpolated normals, UVs, and world coordinates) to the fragment shader.
        * **Fragment Shader:** Runs once per pixel. It receives the interpolated "varying" data and performs the full **Blinn-Phong lighting calculation**:
            1.  **Ambient:** A constant base light.
            2.  **Diffuse:** Light based on the angle between the light source and the pixel's normal.
            3.  **Specular:** Bright highlights based on the angle between the view, light, and surface normal.
    * **Final Output:** The texture color is multiplied by the `(Ambient + Diffuse)` light, and the `Specular` highlight is added on top.
* **Result:** The final, fully shaded, textured, and lit 3D model.

---

## Core Concepts Demonstrated

* **C++:**
    * Object-Oriented Programming (Classes, Structs, Inheritance, Polymorphism)
    * Operator Overloading (for `Vec` and `Mat4f` classes)
    * Standard Template Library (STL), especially `std::vector` and `std::string`
    * File I/O (`std::ifstream`, `std::ofstream`, `std::stringstream`)
    * Memory Management (RAII with `std::vector`)
* **3D Graphics & Math:**
    * Linear Algebra (Vectors, Matrices, Dot Products, Cross Products)
    * 3D Transformation Pipeline (Model, View, Projection matrices)
    * Homogeneous Coordinates (using `Vec4f` and `Mat4f`)
    * Clipping (implied by Z-buffer and projection)
    * Rasterization Algorithms (Barycentric Coordinates)
    * Shading Models (Blinn-Phong)
    * Data Structures (Z-buffer, Image buffer)
    * Geometric Algorithms (Back-Face Culling)