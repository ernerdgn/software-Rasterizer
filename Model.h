#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Vec.h"

struct FaceIndex {
	int v_idx = -1; // vertex index
	int vt_idx = -1; // texture coord index
	int vn_idx = -1; // normal index
};

class Model {
public:
	std::vector<Vec3f> vertices; // list of vertices
	std::vector<Vec2f> uvs; // list of texture coordinates
	std::vector<Vec3f> normals; // list of normals
	std::vector<std::vector<FaceIndex>> faces; // list of faces (each face is a list of vertex indices)

	Model() = default; // def constructor
	Model(const std::string& filename);

private:
	FaceIndex parse_face_index(const std::string& token);
};