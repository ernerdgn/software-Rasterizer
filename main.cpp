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

    // 12 triangle faces
    // back face
    cube.faces.push_back({ 0,3,2 });
	cube.faces.push_back({ 0,2,1 });
	// front face
	cube.faces.push_back({ 4,5,6 });
	cube.faces.push_back({ 4,6,7 });
	// left face
	cube.faces.push_back({ 0,4,7 });
	cube.faces.push_back({ 0,7,3 });
	// right face
	cube.faces.push_back({ 1,2,6 });
	cube.faces.push_back({ 1,6,5 });
	// bottom face
	cube.faces.push_back({ 0,1,5 });
	cube.faces.push_back({ 0,5,4 });
	// top face
	cube.faces.push_back({ 3,7,6 });
	cube.faces.push_back({ 3,6,2 });
    // man, i love github copilot
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
	Mat4f model_matrix = Mat4f::translation({ 0,0,-5 }) * rotation_x * rotation_y;  // move cube back for 5 units

    Vec3f eye_pos = { 0, 0, 0 };
    Vec3f center_pos = { 0, 0, -1 };
    Vec3f up_dir = { 0, 1, 0 };
    Mat4f view_matrix = Mat4f::lookAt(eye_pos, center_pos, up_dir);

    Mat4f projection_matrix = Mat4f::perspective(PI / 3.0f, aspect_ratio, 0.1f, 100.0f);

    Mat4f mvp = projection_matrix * view_matrix * model_matrix;

    // clear buffers
    my_image.clear_buffers();

    // render loop
    for (const auto& face : cube.faces)
    {
		Vec3f v0_world = cube.vertices[face[0]];
		Vec3f v1_world = cube.vertices[face[1]];
		Vec3f v2_world = cube.vertices[face[2]];

		// back face culling
		// transform vertices to screen space
		Vec3f v0_world_tf = (model_matrix * Vec4f(v0_world, 1.0f)).to_vec3f();
		Vec3f v1_world_tf = (model_matrix * Vec4f(v1_world, 1.0f)).to_vec3f();
		Vec3f v2_world_tf = (model_matrix * Vec4f(v2_world, 1.0f)).to_vec3f();
    
		// calculate face normal
		Vec3f normal = (v1_world_tf - v0_world_tf).cross(v2_world_tf - v0_world_tf).normalize();
        
		// calculate view direction
		Vec3f view_dir = (v0_world_tf - eye_pos).normalize();

		// if dot prod positive, cull face
		if (normal.dot(view_dir) >= 0) continue;

        // projection
		// run vertices through mvp matrix
		Vec3f v0_screen = project(v0_world, mvp, width, height);
		Vec3f v1_screen = project(v1_world, mvp, width, height);
		Vec3f v2_screen = project(v2_world, mvp, width, height);

        // draw
        Color face_color = Color(
            50 + (v0_world.x + 1) * 100,
            (v0_world.y + 1) * 127,
            (v0_world.z + 1) * 127
        );

		my_image.drawTriangle(v0_screen, v1_screen, v2_screen, face_color);
    }

    // save
    const std::string filename = "output.tga";

    if (my_image.write_tga_file(filename)) std::cout << "image saved to " << filename << std::endl;
    else std::cerr << "error: cant save the image" << std::endl;

    return 0;
}
