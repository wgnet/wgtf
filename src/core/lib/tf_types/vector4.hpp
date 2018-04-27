#pragma once

#include "vector3.hpp"

namespace wgt
{
class Vector4
{
public:
	float x;
	float y;
	float z;
	float w;

	Vector4() : x(0.f), y(0.f), z(0.f), w(0.f)
	{
	}

	Vector4(const Vector4& v) : x(v.x), y(v.y), z(v.z), w(v.w)
	{
	}

	Vector4(const Vector3& v, float w_) : x(v.x), y(v.y), z(v.z), w(w_)
	{
	}

	Vector4(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_)
	{
	}

	Vector4 operator*(float v) const
	{
		return Vector4(x * v, y * v, z * v, w * v);
	}

	void operator*=(float v)
	{
		x *= v;
		y *= v;
		z *= v;
		w *= v;
	}

	bool operator==(const Vector4& v) const
	{
		return x == v.x && y == v.y && z == v.z && w == v.w;
	}

	bool operator!=(const Vector4& v) const
	{
		return !(*this == v);
	}
};
} // end namespace wgt
