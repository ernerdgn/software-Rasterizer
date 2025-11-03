#pragma once
#include "Vec.h"
#include <cmath>
#include <array>

const float PI = 3.14159265358979323846f;

class Mat4f {
public:
	std::array<std::array<float, 4>, 4> m;

	Mat4f() {  // identity matrix by default
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				m[i][j] = (i == j) ? 1.0f : 0.0f;
	}

	// marix multiplication
    Mat4f operator*(const Mat4f& rhs) const {
        Mat4f result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result.m[i][j] = m[i][0] * rhs.m[0][j] +
                    m[i][1] * rhs.m[1][j] +
                    m[i][2] * rhs.m[2][j] +
                    m[i][3] * rhs.m[3][j];
            }
        }
        return result;
    }

	// matrix-vector multiplication
    Vec4f operator*(const Vec4f& v) const {
        return Vec4f(
            m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w,
            m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
            m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
            m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w
        );
	}

    /* -- static factory functions for transformations -- */
    static Mat4f identity()
    {
        return Mat4f();
	}

    static Mat4f translation(const Vec3f& t)
    {
        Mat4f m;
		m.m[0][3] = t.x;
		m.m[1][3] = t.y;
        m.m[2][3] = t.z;
		return m;
    }

    static Mat4f scaling(const Vec3f& s)
    {
        Mat4f m;
        m.m[0][0] = s.x;
        m.m[1][1] = s.y;
        m.m[2][2] = s.z;
        return m;
    }
   
    static Mat4f rotation_x(float angle_rad)
    {
        Mat4f m;
		float c = std::cos(angle_rad);
		float s = std::sin(angle_rad);
		m.m[1][1] = c;  m.m[1][2] = -s;
		m.m[2][1] = s;  m.m[2][2] = c;
		return m;
    }

    static Mat4f rotation_y(float angle_rad)
    {
		Mat4f m;
		float c = std::cos(angle_rad);
		float s = std::sin(angle_rad);
		m.m[0][0] = c;  m.m[0][2] = s;
		m.m[2][0] = -s; m.m[2][2] = c;
        return m;
    }

    static Mat4f rotation_z(float angle_rad)
    {
        Mat4f m;
        float c = std::cos(angle_rad);
        float s = std::sin(angle_rad);
        m.m[0][0] = c;  m.m[0][1] = -s;
        m.m[1][0] = s;  m.m[1][1] = c;
        return m;
    }

	// view (lookAt) matrix
    static Mat4f lookAt(const Vec3f& eye, const Vec3f& center, const Vec3f& up)
    {
        Vec3f z_axis = (eye - center).normalize();
        Vec3f x_axis = up.cross(z_axis).normalize();
		Vec3f y_axis = z_axis.cross(x_axis).normalize();
        
        Mat4f m;
        m.m[0][0] = x_axis.x; m.m[0][1] = x_axis.y; m.m[0][2] = x_axis.z; m.m[0][3] = -x_axis.dot(eye);
        m.m[1][0] = y_axis.x; m.m[1][1] = y_axis.y; m.m[1][2] = y_axis.z; m.m[1][3] = -y_axis.dot(eye);
        m.m[2][0] = z_axis.x; m.m[2][1] = z_axis.y; m.m[2][2] = z_axis.z; m.m[2][3] = -z_axis.dot(eye);
        return m;
	}

	// perspective projection matrix
    static Mat4f perspective(float fov_y_rad, float aspect_ratio, float near, float far)
    {
        Mat4f m = {};
		float tan_half_fov = std::tan(fov_y_rad / 2.0f);
        m.m[0][0] = 1.0f / (aspect_ratio * tan_half_fov);
        m.m[1][1] = 1.0f / tan_half_fov;
        m.m[2][2] = -(far + near) / (far - near);
        m.m[2][3] = -(2.0f * far * near) / (far - near);
        m.m[3][2] = -1.0f;
        return m;
	}
};