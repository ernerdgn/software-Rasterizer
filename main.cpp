#include <iostream>
#include <vector>
#include "Image.h"
#include "Color.h"
#include "Vec.h"
#include "Mat4f.h"
#include "Model.h"

// hard-coded cube model
Model create_cube() {
    Model cube;
    // 8 vertices of the cube
    cube.vertices.push_back({ -1, -1, -1 }); // 0
    cube.vertices.push_back({ 1, -1, -1 }); // 1
    cube.vertices.push_back({ 1,  1, -1 }); // 2
    cube.vertices.push_back({ -1,  1, -1 }); // 3
    cube.vertices.push_back({ -1, -1,  1 }); // 4
    cube.vertices.push_back({ 1, -1,  1 }); // 5
    cube.vertices.push_back({ 1,  1,  1 }); // 6
    cube.vertices.push_back({ -1,  1,  1 }); // 7

    // 12 edges
    cube.faces.push_back({ 0, 1 }); // back face
    cube.faces.push_back({ 1, 2 });
    cube.faces.push_back({ 2, 3 });
    cube.faces.push_back({ 3, 0 });
    cube.faces.push_back({ 4, 5 }); // front face
    cube.faces.push_back({ 5, 6 });
    cube.faces.push_back({ 6, 7 });
    cube.faces.push_back({ 7, 4 });
    cube.faces.push_back({ 0, 4 }); // connecting edges
    cube.faces.push_back({ 1, 5 });
    cube.faces.push_back({ 2, 6 });
    cube.faces.push_back({ 3, 7 });

    return cube;
}

Vec3f project(const Vec3f& v, const Mat4f& mvp, int width, int height) {
    Vec4f clip_space = mvp * Vec4f(v, 1.0f);

    float w = clip_space.w;
    Vec3f ndc = clip_space.to_vec3f();

    float screen_x = (ndc.x + 1.0f) * 0.5f * width;
    float screen_y = (1.0f - ndc.y) * 0.5f * height; // Flip Y

    return { screen_x, screen_y, w };
}

int main() {
    const int width = 800;
    const int height = 600;
    const float aspect_ratio = (float)width / (float)height;

    Image my_image(width, height);

    Model cube = create_cube();

    Mat4f rotation_x = Mat4f::rotation_x(PI / 4.0f); // 45 degrees
    Mat4f rotation_y = Mat4f::rotation_y(PI / 4.0f);
    Mat4f model_matrix = rotation_x * rotation_y;

    Vec3f eye_pos = { 0, 0, 5 };
    Vec3f center_pos = { 0, 0, 0 };
    Vec3f up_dir = { 0, 1, 0 };
    Mat4f view_matrix = Mat4f::lookAt(eye_pos, center_pos, up_dir);

    Mat4f projection_matrix = Mat4f::perspective(PI / 3.0f, aspect_ratio, 0.1f, 100.0f);

    Mat4f mvp = projection_matrix * view_matrix * model_matrix;

    for (const auto& face : cube.faces) {
        Vec3f v0_world = cube.vertices[face[0]];
        Vec3f v1_world = cube.vertices[face[1]];

        Vec3f v0_screen = project(v0_world, mvp, width, height);
        Vec3f v1_screen = project(v1_world, mvp, width, height);

        my_image.drawLine(
            static_cast<int>(v0_screen.x), static_cast<int>(v0_screen.y),
            static_cast<int>(v1_screen.x), static_cast<int>(v1_screen.y),
            white
        );
    }

    // save
    const std::string filename = "output.tga";

    if (my_image.write_tga_file(filename)) std::cout << "image saved to " << filename << std::endl;
    else std::cerr << "error: cant save the image" << std::endl;

    return 0;
}
