//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  linear_interpolator.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "linear_interpolator.hpp"
#include "models/bezier_point.hpp"
#include "models/point.hpp"

namespace wgt
{
BezierPointData LinearInterpolator::interpolate(float time, const BezierPoint& p1, const BezierPoint& p2)
{
	auto pos = *p1.pos.get() + (*p2.pos.get() - *p1.pos.get()) * time;
	BezierPointData data = { { pos.getX(), pos.getY() }, { 0.0f, 0.0f }, { 0.0f, 0.0f } };
	return data;
}

float LinearInterpolator::timeAtX(float x, const BezierPoint& p1, const BezierPoint& p2)
{
	assert(x >= p1.pos->getX() && x <= p2.pos->getX());
	if (p2.pos->getX() == p1.pos->getX())
		return 1.0;
	return (x - p1.pos->getX()) / (p2.pos->getX() - p1.pos->getX());
}

void LinearInterpolator::updateControlPoints(BezierPoint& point, BezierPoint* prevPoint, BezierPoint* nextPoint)
{
	if (prevPoint)
	{
		prevPoint->cp1->setX(0.f);
		prevPoint->cp1->setY(0.f);
		prevPoint->cp2->setX(0.f);
		prevPoint->cp2->setY(0.f);
	}
	if (nextPoint)
	{
		nextPoint->cp1->setX(0.f);
		nextPoint->cp1->setY(0.f);
		nextPoint->cp2->setX(0.f);
		nextPoint->cp2->setY(0.f);
	}
}
} // end namespace wgt
