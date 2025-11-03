#pragma once
#include <vector>
#include <string>
#include "Color.h"
#include <limits> //std::numeric_limits
#include "Vec.h"
#include "Texture.h"

class Image {
public:
	Image(int width, int height);  // const, blank img
	// single pixel color setter
	bool set_pixel(int x, int y, float z, const Color& c);
	// draw line, bresenham's algo
	// void drawLine(int x0, int y0, int x1, int y1, const Color& c);
	// draw a filled triangle
	void drawTriangle(Vec3f v_screen[3], Vec2f uvs[3], const Texture& texture);
	// clear color and depth buffers
	void clear_buffers();
	// wrt img to .tga file
	bool write_tga_file(const std::string& filename, bool v_flip = false);


	// getters
	int get_width() const { return m_width; }
	int get_height() const { return m_height; }


private:
	int m_width;
	int m_height;
	std::vector<Color> m_buffer; // vector of pixel data
	std::vector<float> m_zbuffer; // depth buffer for z-buffering
};