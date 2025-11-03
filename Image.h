#pragma once
#include <vector>
#include <string>
#include "Color.h"

class Image {
public:
	Image(int width, int height);  // const, blank img
	// single pixel color setter
	bool set_pixel(int x, int y, const Color& c);
	// draw line, bresenham's algo
	void drawLine(int x0, int y0, int x1, int y1, const Color& c);
	// wrt img to .tga file
	bool write_tga_file(const std::string& filename, bool v_flip = false);


	// getters
	int get_width() const { return m_width; }
	int get_height() const { return m_height; }


private:
	int m_width;
	int m_height;
	std::vector<Color> m_buffer; // vector of pixel data
};