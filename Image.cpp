#include "Image.h"
#include <fstream>
#include <iostream>

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


