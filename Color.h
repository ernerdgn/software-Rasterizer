#pragma once
#include <cstdint>  //std::uint8_t

struct Color {
	std::uint8_t b = 0;
	std::uint8_t g = 0;
	std::uint8_t r = 0;
	std::uint8_t a = 255;  //rgba
	// TGA files expect in bgr order

	Color() = default;
	Color(std::uint8_t r,
		std::uint8_t g, 
		std::uint8_t b, 
		std::uint8_t a=255) : b(b), g(g), r(r), a(a) {}
};

// predefined colors
const Color white = Color(255, 255, 255);
const Color red = Color(255, 0, 0);
const Color green = Color(0, 255, 0);
const Color blue = Color(0, 0, 255);
const Color black = Color(0, 0, 0);
