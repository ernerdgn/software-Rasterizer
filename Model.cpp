#include "Model.h"

Model::Model(const std::string& filename)
{
	std::ifstream in(filename);
	if (!in)
	{
		std::cerr << "error: cant open " << filename << std::endl;
		return;
	}

	std::string line;
	while (std::getline(in, line))
	{
		std::stringstream ss(line);
		std::string prefix;
		ss >> prefix;

		if (prefix == "v") // vertex
		{
			Vec3f v;
			ss >> v.x >> v.y >> v.z;
			v.y = -v.y; // invert y for right-handed coord system
			vertices.push_back(v);
		}
		else if (prefix == "vt") // texture coord
		{
			Vec2f uv;
			ss >> uv.x >> uv.y;
			uvs.push_back(uv);
		}
		else if (prefix == "vn") // normal
		{
			Vec3f n;
			ss >> n.x >> n.y >> n.z;
			normals.push_back(n.normalize());
		}
		else if (prefix == "f") // face
		{
			std::vector<FaceIndex> face;
			std::string token;
			while (ss >> token)
			{
				face.push_back(parse_face_index(token));
			}
			faces.push_back(face);
		}
	}

	in.close();

	std::cout << "model loaded: " << filename
		<< " | vertices: " << vertices.size()
		<< " | uvs: " << uvs.size()
		<< " | normals: " << normals.size()
		<< " | faces: " << faces.size() << std::endl;
}

FaceIndex Model::parse_face_index(const std::string& token)
{
	FaceIndex idx;
	std::stringstream ss(token);
	std::string part;
	int i = 0;

	while (std::getline(ss, part, '/'))
	{
		if (!part.empty())
		{
			int index = std::stoi(part) - 1; // OBJ indices are 1-based, convert to 0-based
			if (i == 0) idx.v_idx = index;
			else if (i == 1) idx.vt_idx = index;
			else if (i == 2) idx.vn_idx = index;
		}
		i++;
	}

	return idx;
}
