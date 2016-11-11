//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  point.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "point.hpp"

namespace wgt
{
Point::Point() : x(0.f), y(0.f)
{
}

Point::Point(float x, float y) : x(x), y(y)
{
}

Point::Point(const Point& other) : x(other.x), y(other.y)
{
}

Point::Point(Point&& other)
    : xChanged(std::move(other.xChanged)), yChanged(std::move(other.yChanged)), x(other.x), y(other.y)
{
}

Point& Point::operator=(const Point& other)
{
	x = other.x;
	y = other.y;
	return *this;
}

Point& Point::operator=(Point&& other)
{
	x = other.x;
	y = other.y;

	xChanged = std::move(other.xChanged);
	yChanged = std::move(other.yChanged);

	return *this;
}

void Point::setX(const float& val)
{
	auto old = x;
	x = val;
	if (old != val)
		xChanged(old, x);
}

void Point::setY(const float& val)
{
	auto old = y;
	y = val;
	if (old != val)
		yChanged(old, y);
}

Point& Point::operator*=(float val)
{
	auto oldX = x;
	auto oldY = y;
	x *= val;
	y *= val;
	if (oldX != x)
		xChanged(oldX, x);
	if (oldY != y)
		yChanged(oldY, y);
	return *this;
}

Point& Point::operator/=(float val)
{
	auto oldX = x;
	auto oldY = y;
	x /= val;
	y /= val;
	if (oldX != x)
		xChanged(oldX, x);
	if (oldY != y)
		yChanged(oldY, y);
	return *this;
}

Point& Point::operator+=(const Point& other)
{
	auto oldX = x;
	auto oldY = y;
	x += other.x;
	y += other.y;
	if (oldX != x)
		xChanged(oldX, x);
	if (oldY != y)
		yChanged(oldY, y);
	return *this;
}

Point& Point::operator-=(const Point& other)
{
	auto oldX = x;
	auto oldY = y;
	x -= other.x;
	y -= other.y;
	if (oldX != x)
		xChanged(oldX, x);
	if (oldY != y)
		yChanged(oldY, y);
	return *this;
}

Point operator-(const Point& lhs, const Point& rhs)
{
	return Point(lhs.getX() - rhs.getX(), lhs.getY() - rhs.getY());
}
Point operator+(const Point& lhs, const Point& rhs)
{
	return Point(lhs.getX() + rhs.getX(), lhs.getY() + rhs.getY());
}

Point operator*(float val, const Point& rhs)
{
	return Point(rhs.getX() * val, rhs.getY() * val);
}

Point operator*(const Point& lhs, float val)
{
	return val * lhs;
}

Point operator/(const Point& lhs, float val)
{
	return Point(lhs.getX() / val, lhs.getY() / val);
}

bool operator==(const Point& p1, const Point& p2)
{
	return p1.getX() == p2.getX() && p1.getY() == p2.getY();
}
} // end namespace wgt
