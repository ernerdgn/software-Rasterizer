#pragma once
#include <vector>
#include "Vec.h"

class Model {
public:
	std::vector<Vec3f> vertices; // list of vertices
	std::vector<std::vector<int>> faces; // list of faces (each face is a list of vertex indices)

	Model() = default; // def constructor
};