#pragma once
#include "Vec.h"
#include "Texture.h"
#include "Color.h"
#include "Model.h"
#include "Mat4f.h"

class IShader {
public:
	virtual ~IShader() {}
	
	// vertex shader
	virtual Vec4f vertex(int face_idx, int vert_idx) = 0;

	// fragment shader
	virtual bool fragment(const Vec3f& bary_coords, Color& out_color) = 0;
};
