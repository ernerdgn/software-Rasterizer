#include "Image.h"
#include <fstream>
#include <iostream>
#include <algorithm> //std::swap
#include <cmath> //std::abs

Image::Image(int width, int height) : m_width(width), m_height(height)
{
	m_buffer.resize(m_width * m_height, black);
	m_zbuffer.resize(m_width * m_height, std::numeric_limits<float>::infinity());
}

bool Image::set_pixel(int x, int y, float z, const Color& c)
{
	if (x < 0 || x >= m_width || y < 0 || y >= m_height) return false;

	int index = y * m_width + x;
	if (z < m_zbuffer[index]) // pixel is closer
	{
		m_zbuffer[index] = z;
		m_buffer[index] = c;
		return true;
	}

	return false; // pixel was occluded
}

// A Fast Voxel Traversal Algorithm for Ray Tracing
// by John Amanatides and Andrew Woo, August 1987
//void Image::drawLine(int x0, int y0, int x1, int y1, const Color& c)
//{
//	bool steep = false;
//	if (std::abs(x0 - x1) < std::abs(y0 - y1))
//	{
//		std::swap(x0, y0);
//		std::swap(x1, y1);
//		steep = true;
//	}
//
//	if (x0 > x1)
//	{
//		std::swap(x0, x1);
//		std::swap(y0, y1);
//	}
//
//	int dx = x1 - x0;
//	int dy = y1 - y0;
//
//	int derror2 = std::abs(dy) * 2;
//	int error2 = 0;
//
//	// determine y step direction
//	int ystep = (y1 > y0) ? 1 : -1;
//	int y = y0;
//
//	for (int x = x0; x <= x1; x++)
//	{
//		if (steep) set_pixel(y, x, c);
//		else set_pixel(x, y, c);
//
//		error2 += derror2;
//
//		if (error2 > dx)
//		{
//			y += ystep;
//			error2 -= dx * 2;
//		}
//	}
//}

Vec3f barycentric(const Vec3f* pts, int x, int y)
{
	Vec3f u = Vec3f(pts[2].x - pts[0].x, pts[1].x - pts[0].x, pts[0].x - x).cross(
		Vec3f(pts[2].y - pts[0].y, pts[1].y - pts[0].y, pts[0].y - y));

	if (std::abs(u.z) < 1.0f) // triangle is degenerate
		return { -1,-1,-1 };

	float u_coord = 1.0f - (u.x + u.y) / u.z;
	float v_coord = u.y / u.z;
	float w_coord = u.x / u.z;

	return { u_coord, v_coord, w_coord };
}

void Image::drawTriangle(Vec3f v_screen[3], IShader& shader)
{
	// bounding box
	int min_x = static_cast<int>(std::max(0.0f, std::min({ v_screen[0].x, v_screen[1].x, v_screen[2].x })));
	int max_x = static_cast<int>(std::min((float)m_width - 1, std::max({ v_screen[0].x, v_screen[1].x, v_screen[2].x })));
	int min_y = static_cast<int>(std::max(0.0f, std::min({ v_screen[0].y, v_screen[1].y, v_screen[2].y })));
	int max_y = static_cast<int>(std::min((float)m_height - 1, std::max({ v_screen[0].y, v_screen[1].y, v_screen[2].y })));

	for (int y = min_y; y <= max_y; y++)
	{
		for (int x = min_x; x <= max_x; x++)
		{

			// barycentric coords
			Vec3f bc_coords = barycentric(v_screen, x, y);
			if (bc_coords.x < 0 || bc_coords.y < 0 || bc_coords.z < 0) continue;

			// interpolate depth
			float w_interpolated = bc_coords.x * v_screen[0].z +
				bc_coords.y * v_screen[1].z +
				bc_coords.z * v_screen[2].z;

			// call fragment shader
			Color final_color;
			// draw pixel if fragment shader returns true
			if (shader.fragment(bc_coords, final_color)) set_pixel(x, y, w_interpolated, final_color);
		}
	}
}

void Image::clear_buffers()
{
	std::fill(m_buffer.begin(), m_buffer.end(), black);
	std::fill(m_zbuffer.begin(), m_zbuffer.end(), std::numeric_limits<float>::infinity());
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


