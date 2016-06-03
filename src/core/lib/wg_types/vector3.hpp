#ifndef VECTOR3_HPP
#define VECTOR3_HPP

namespace wgt
{
class Vector3
{
public:
	Vector3() : x(0.f), y(0.f), z(0.f) {}
	Vector3(const Vector3& v) : x(v.x), y(v.y), z(v.z) {}
	Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
	float x;
	float y;
	float z;

	bool operator == (const Vector3& v) const
	{
		return x == v.x && y == v.y && z == v.z;
	}

	bool operator != (const Vector3& v) const
	{
		return !(*this == v);
	}
};
} // end namespace wgt
#endif // VECTOR3_HPP
