//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  point.hpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef POINT_H_
#define POINT_H_

#pragma once

#include "core_common/signal.hpp"

namespace wgt
{
class Point
{
public:
	Point();
	Point(float x, float y);
	Point(const Point& other);
	Point(Point&& other);

	Point& operator=( const Point& other );
	Point& operator=( Point&& other );

	float getX() const { return x; }
	void setX(const float& val);
	float getY() const { return y; }
	void setY(const float& val);

	Point& operator*=( float val );
	Point& operator/=( float val );
	Point& operator+=( const Point& other );
	Point& operator-=( const Point& other );

	Signal<void(float, float)> xChanged;
	Signal<void(float, float)> yChanged;
private:
	float x;
	float y;
};

Point operator-( const Point& lhs, const Point& rhs );
Point operator+( const Point& lhs, const Point& rhs );
Point operator/( const Point& lhs, float val );
Point operator*( const Point& lhs, float val );
Point operator*( float val, const Point& rhs );
bool operator==( const Point& lhs, const Point& rhs );
} // end namespace wgt
#endif // POINT_H_
