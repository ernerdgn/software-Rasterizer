#pragma once
#include <vector>
#include <string>
#include "Color.h"
#include <fstream>
#include <iostream>

class Texture {
public:
	Texture() = default;

	// load texture from TGA file
	bool load_tga_file(const std::string& filename);

	// get color at uv coords (u,v in [0,1])
	Color sample(float u, float v) const;

private:
	int m_width = 0;
	int m_height = 0;
	int m_bytes_per_pixel = 0;
	std::vector<std::uint8_t> m_buffer;  // raw pixel data
};