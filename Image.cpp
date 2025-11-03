#include "Image.h"
#include <fstream>
#include <iostream>
#include <algorithm> //std::swap
#include <cmath> //std::abs

Image::Image(int width, int height) : m_width(width), m_height(height)
{
	m_buffer.resize(m_width * m_height, black);
}

bool Image::set_pixel(int x, int y, const Color& c)
{
	if (x < 0 || x >= m_width || y < 0 || y >= m_height) return false;

	m_buffer[y * m_width + x] = c;
	return true;
}

// A Fast Voxel Traversal Algorithm for Ray Tracing
// by John Amanatides and Andrew Woo, August 1987
void Image::drawLine(int x0, int y0, int x1, int y1, const Color& c)
{
	bool steep = false;
	if (std::abs(x0 - x1) < std::abs(y0 - y1))
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}

	if (x0 > x1)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	int dx = x1 - x0;
	int dy = y1 - y0;

	int derror2 = std::abs(dy) * 2;
	int error2 = 0;

	// determine y step direction
	int ystep = (y1 > y0) ? 1 : -1;
	int y = y0;

	for (int x = x0; x <= x1; x++)
	{
		if (steep) set_pixel(y, x, c);
		else set_pixel(x, y, c);

		error2 += derror2;

		if (error2 > dx)
		{
			y += ystep;
			error2 -= dx * 2;
		}
	}
}

bool Image::write_tga_file(const std::string& filename, bool v_flip)
{
	std::ofstream out(filename, std::ios::binary);
	if (!out)
	{
		std::cerr << "error: cant open file " << filename << " to write" << std::endl;
		return false;
	}

	// tga header (18 bytes)
	std::uint8_t header[18] = { 0 };
	header[2] = 2; // img type 2, uncompressed true-color
	header[12] = m_width & 0xFF; // width, low byte
	header[13] = (m_width >> 8) & 0xFF; // width, high byte
	header[14] = m_height & 0xFF; // height, low byte
	header[15] = (m_height >> 8) & 0xFF; // height, high byte
	header[16] = 24; // bits per-pixel (24=r,g,b)

	// set img descriptor (byte 17)
	if (v_flip) header[17] = 0x20; // 00100000 flips vertically
	else header[17] = 0x00; // 00000000

	out.write(reinterpret_cast<char*>(header), sizeof(header));

	// pixel data, bgr order
	for (const auto& color : m_buffer)
	{
		out.put(color.b);
		out.put(color.g);
		out.put(color.r);
	}

	out.close();
	if (!out)
	{
		std::cerr << "error: something bad happened while writing " << filename << std::endl;
		return false;
	}
	
	return true;
}


