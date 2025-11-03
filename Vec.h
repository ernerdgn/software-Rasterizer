#pragma once
#include <cmath>
#include <iostream>

// vec2i for screen coord
struct Vec2i {
	int x = 0;
	int y = 0;
};

// vec3f for 3d coord
struct Vec3f {
	float x = 0;
	float y = 0;
	float z = 0;

	// vec/vec ops
	Vec3f operator+(const Vec3f& v) const { return { x + v.x, y + v.y, z + v.z }; }
	Vec3f operator-(const Vec3f& v) const { return { x - v.x, y - v.y, z - v.z }; }
	
	// vec/scalar ops
	Vec3f operator*(float f) const { return { x * f, y * f, z * f }; }
	Vec3f operator/(float f) const { return { x / f, y / f, z / f }; }

	// dot-cross products
	float dot(const Vec3f& v) const { return x * v.x + y * v.y + z * v.z; }
	Vec3f cross(const Vec3f& v) const {
		return { y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x };
	}

	// magnitude 'n normalization
	float length() const { return std::sqrt(dot(*this)); }
	Vec3f normalize() const { return *this / length(); }
};

struct Vec4f {
	float x = 0;
	float y = 0;
	float z = 0;
	float w = 1;

	Vec4f() = default;
	Vec4f(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	Vec4f(const Vec3f& v, float w_val) : x(v.x), y(v.y), z(v.z), w(w_val) {}

	Vec3f to_vec3f() const {
		if (w == 0)	return { x, y, z }; // avoid div by 0
		return { x / w, y / w, z / w };
	}
};

