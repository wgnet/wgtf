#pragma once

#include "vector3.hpp"
#include <cmath>

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

	void lerp( const Vector4 & v1, const Vector4 & v, float t )
	{
		*this = v1 + (v - v1) * t;
	}

	Vector4 operator +( const Vector4& v ) const
	{
		return Vector4( x+v.x, y+v.y, z+v.z, w+v.w );
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

	Vector4 operator -( const Vector4& v ) const
	{
		return Vector4( x-v.x, y-v.y, z-v.z, w-v.w );
	}

	bool operator==(const Vector4& v) const
	{
		return x == v.x && y == v.y && z == v.z && w == v.w;
	}

	bool operator!=(const Vector4& v) const
	{
		return !(*this == v);
	}
 
	bool almostEqual(const Vector4& v, float epsilon) const
	 {
	  return 
			almostEqual( x, v.x, epsilon ) &&
			almostEqual( y, v.y, epsilon ) &&
			almostEqual( z, v.z, epsilon ) &&
			almostEqual( w, v.w, epsilon );
	 }

private:
	static bool almostEqual( const float f1, const float f2, const float epsilon = 0.0004f )
	{
		return fabsf( f1 - f2 ) < epsilon;
	}
};
} // end namespace wgt
