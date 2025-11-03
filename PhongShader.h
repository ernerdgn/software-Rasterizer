#pragma once
#include "IShader.h"

class PhongShader : public IShader {
public:
	// data from vertex shader to fragment shader
	Vec2f varying_uvs[3];
	Vec3f varying_normals[3];
	Vec3f varying_world_coords[3];

	// uniforms
	const Model* model = nullptr;
	const Texture* texture = nullptr;
	Mat4f uniform_mvp;
	Mat4f uniform_model_matrix;
	Vec3f uniform_light_pos;
	Vec3f uniform_camera_pos;

	// vertex shader
	virtual Vec4f vertex(int face_idx, int vert_idx) override;

	// fragment shader
	virtual bool fragment(const Vec3f& bary_coords, Color& out_color) override;
};