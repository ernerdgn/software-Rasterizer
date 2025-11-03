#include "PhongShader.h"

Vec4f PhongShader::vertex(int face_idx, int vert_idx)
{
	// get the vertex index from the face
	const FaceIndex& face = model->faces[face_idx][vert_idx];
	Vec3f v_world = model->vertices[face.v_idx];
	Vec2f uv = model->uvs[face.vt_idx];
	Vec3f normal = model->normals[face.vn_idx];

	// transform
	// world coords, for lighting
	Vec4f world_pos = uniform_model_matrix * Vec4f(v_world, 1.0f);

	// clip-space coords, for rasterizer
	Vec4f clip_pos = uniform_mvp * Vec4f(v_world, 1.0f);

	// store varying data for vertex
	varying_world_coords[vert_idx] = world_pos.to_vec3f();
	varying_uvs[vert_idx] = uv;
	varying_normals[vert_idx] = (uniform_model_matrix * Vec4f(normal, .0f)).to_vec3f().normalize();

	return clip_pos;
}

bool PhongShader::fragment(const Vec3f& bary_coords, Color& out_color)
{
	// interpolate varying data using barycentric coordinates
    Vec2f uv = varying_uvs[0] * bary_coords.x +
        varying_uvs[1] * bary_coords.y +
        varying_uvs[2] * bary_coords.z;

    Vec3f normal = (varying_normals[0] * bary_coords.x +
        varying_normals[1] * bary_coords.y +
        varying_normals[2] * bary_coords.z).normalize();

    Vec3f world_pos = varying_world_coords[0] * bary_coords.x +
        varying_world_coords[1] * bary_coords.y +
        varying_world_coords[2] * bary_coords.z;

	// sample texture color
    Color texture_color = texture->sample(uv.x, uv.y);

	// Blinn-Phong lighting model

	// light properties
    float ambient_strength = 0.2f;
    float diffuse_strength = 0.8f;
    float specular_strength = 1.0f;
    float shininess = 32.0f;
	Vec3f light_color = { 1.0f, 1.0f, 1.0f }; // white light

    // ambient
    Vec3f ambient = light_color * ambient_strength;

    // diffuse
    Vec3f light_dir = (uniform_light_pos - world_pos).normalize();
    float diff = std::max(0.0f, normal.dot(light_dir));
    Vec3f diffuse = light_color * diff * diffuse_strength;

    // specular
    Vec3f view_dir = (uniform_camera_pos - world_pos).normalize();
    Vec3f half_dir = (light_dir + view_dir).normalize();
    float spec = std::pow(std::max(0.0f, normal.dot(half_dir)), shininess);
    Vec3f specular = light_color * spec * specular_strength;

	// combine results
    Vec3f base_color = { texture_color.r / 255.f, texture_color.g / 255.f, texture_color.b / 255.f };
    Vec3f final_rgb = (base_color * (ambient + diffuse)) + (specular * 0.5f);

	// clamp final color to [0,1]
    final_rgb.x = std::min(1.0f, final_rgb.x);
    final_rgb.y = std::min(1.0f, final_rgb.y);
    final_rgb.z = std::min(1.0f, final_rgb.z);

	// convert to Color
    out_color = Color(
        static_cast<std::uint8_t>(final_rgb.x * 255),
        static_cast<std::uint8_t>(final_rgb.y * 255),
        static_cast<std::uint8_t>(final_rgb.z * 255)
    );

    return true; // true = draw this pixel
}

