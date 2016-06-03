#ifndef VECTOR2_HPP
#define VECTOR2_HPP

namespace wgt
{
class Vector2
{
public:
	Vector2() : x(0.f), y(0.f) {}
	Vector2(const Vector2& v) : x(v.x), y(v.y) {}
	Vector2(float x_, float y_) : x(x_), y(y_) {}
	float x;
	float y;

	bool operator == (const Vector2& v) const
	{
		return x == v.x && y == v.y;
	}
};
} // end namespace wgt
#endif // VECTOR2_HPP
