#include <iostream>
#include <vector>
#include "Image.h"
#include "Color.h"
#include "Vec.h"
#include "Mat4f.h"
#include "Model.h"
#include "Texture.h"
#include "PhongShader.h"

// hard-coded cube model
//Model create_cube() {
//    Model cube;
//    // 8 vertices of the cube
//    cube.vertices.push_back({ -1, -1, -1 }); // 0
//    cube.vertices.push_back({ 1, -1, -1 }); // 1
//    cube.vertices.push_back({ 1,  1, -1 }); // 2
//    cube.vertices.push_back({ -1,  1, -1 }); // 3
//    cube.vertices.push_back({ -1, -1,  1 }); // 4
//    cube.vertices.push_back({ 1, -1,  1 }); // 5
//    cube.vertices.push_back({ 1,  1,  1 }); // 6
//    cube.vertices.push_back({ -1,  1,  1 }); // 7
//
//    // 12 triangle faces
//    // back face
//    cube.faces.push_back({ 0,3,2 });
//	cube.faces.push_back({ 0,2,1 });
//	// front face
//	cube.faces.push_back({ 4,5,6 });
//	cube.faces.push_back({ 4,6,7 });
//	// left face
//	cube.faces.push_back({ 0,4,7 });
//	cube.faces.push_back({ 0,7,3 });
//	// right face
//	cube.faces.push_back({ 1,2,6 });
//	cube.faces.push_back({ 1,6,5 });
//	// bottom face
//	cube.faces.push_back({ 0,1,5 });
//	cube.faces.push_back({ 0,5,4 });
//	// top face
//	cube.faces.push_back({ 3,7,6 });
//	cube.faces.push_back({ 3,6,2 });
//    // man, i love github copilot
//    return cube;
//}

Vec3f project(const Vec3f& v, const Mat4f& mvp, int width, int height) {
    Vec4f clip_space = mvp * Vec4f(v, 1.0f);

    float w = clip_space.w;
    Vec3f ndc = clip_space.to_vec3f();

    float screen_x = (ndc.x + 1.0f) * 0.5f * width;
    float screen_y = (1.0f - ndc.y) * 0.5f * height; // Flip Y

    return { screen_x, screen_y, w };
}

int main()
{
    const int width = 800;
    const int height = 800;
    const float aspect_ratio = (float)width / (float)height;

    Image my_image(width, height);

	// load model and texture
    Model model("african_head.obj");
    Texture texture;
    if (!texture.load_tga_file("african_head_diffuse_uncomp.tga")) return -1;

    // transformations
    Vec3f eye_pos = { 0, 0, 3 };
    Vec3f center_pos = { 0, 0, 0 };
    Vec3f up_dir = { 0, 1, 0 };
    //Vec3f light_pos = { 1, 1, 3 }; // light pos
    //Vec3f light_pos = { 0, -1, 3 };  // spooky
    Vec3f light_pos = { 5, 1, 3 };  // from right

    Mat4f model_matrix = Mat4f::identity();
    Mat4f view_matrix = Mat4f::lookAt(eye_pos, center_pos, up_dir);
    Mat4f projection_matrix = Mat4f::perspective(PI / 3.0f, aspect_ratio, 0.1f, 100.0f);
    Mat4f mvp = projection_matrix * view_matrix * model_matrix;

	// shader setup
    PhongShader shader;
    shader.model = &model;
    shader.texture = &texture;
    shader.uniform_mvp = mvp;
    shader.uniform_model_matrix = model_matrix;
    shader.uniform_light_pos = light_pos;
    shader.uniform_camera_pos = eye_pos;

    // clear buffers
    my_image.clear_buffers();

	// render loop
    for (int i = 0; i < model.faces.size(); ++i)
    {
        const auto& face_indices = model.faces[i];
        if (face_indices.size() != 3) continue;

		// run vertex shader for each vertex of the triangle
        Vec4f clip_coords[3];
        for (int j = 0; j < 3; ++j) clip_coords[j] = shader.vertex(i, j);

		// project to screen space
        Vec3f v_screen[3];
        for (int j = 0; j < 3; ++j)
        {
            Vec3f ndc = clip_coords[j].to_vec3f();
            float screen_x = (ndc.x + 1.0f) * 0.5f * width;
            float screen_y = (1.0f - ndc.y) * 0.5f * height; // flip Y
            v_screen[j] = { screen_x, screen_y, clip_coords[j].w }; // store w for depth
        }

		// back-face culling
        Vec3f v0_screen = { v_screen[0].x, v_screen[0].y, 0 };
        Vec3f v1_screen = { v_screen[1].x, v_screen[1].y, 0 };
        Vec3f v2_screen = { v_screen[2].x, v_screen[2].y, 0 };
        Vec3f normal_screen = (v1_screen - v0_screen).cross(v2_screen - v0_screen).normalize();

        if (normal_screen.z < 0) continue; // cull

        // draw triangle
        my_image.drawTriangle(v_screen, shader);
    }

    // save
    const std::string filename = "output.tga";
    if (my_image.write_tga_file(filename, false)) std::cout << "Image saved successfully to " << filename << std::endl;
    else std::cerr << "Error saving image." << std::endl;

    return 0;
}
