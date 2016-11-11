//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  cubic_bezier_interpolator.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "cubic_bezier_interpolator.hpp"
#include "models/bezier_point.hpp"
#include "models/point.hpp"

namespace wgt
{
/*! Computes the Cubic Bezier position for the specified positions and control points at the given time t
*/
template <class TVal>
TVal computeValueAtT(float t, TVal p1, TVal c1, TVal c2, TVal p2)
{
	auto t2 = t * t;
	auto t3 = t2 * t;
	// Cubic Bezier: (1-t)^3 * P1 + 3(1-t)^2 * t * C1 + 3*(1-t)*t^2*C2 + t^3*P2
	auto invT = (1 - t);
	auto invT2 = invT * invT;
	auto invT3 = invT2 * invT;
	return invT3 * p1 + 3.f * invT2 * t * c1 + 3.f * invT * t2 * c2 + t3 * p2;
}

BezierPointData CubicBezierInterpolator::interpolate(float time, const BezierPoint& p1, const BezierPoint& p2)
{
	auto pos1 = *p1.pos.get();
	auto c1 = pos1 + *p1.cp2.get();
	auto pos2 = *p2.pos.get();
	auto c2 = pos2 + *p2.cp1.get();
	auto newPos = computeValueAtT(time, pos1, c1, c2, pos2);
	// Using DeCastlejau's to compute new control points
	auto cpos1 = (c1 - pos1) * time + pos1;
	auto cpos2 = (c2 - c1) * time + c1;
	auto cp3 = (pos2 - c2) * time + c2;
	auto newCpos1 = (cpos2 - cpos1) * time + cpos1 - newPos;
	auto newCpos2 = (cp3 - cpos2) * time + cpos2 - newPos;
	BezierPointData data = {
		{ newPos.getX(), newPos.getY() }, { newCpos1.getX(), newCpos1.getY() }, { newCpos2.getX(), newCpos2.getY() }
	};
	return data;
}

float CubicBezierInterpolator::timeAtX(float x, const BezierPoint& p1, const BezierPoint& p2)
{
	const auto& prevPos = *p1.pos.get();
	const auto& prevCp2 = *p1.cp2.get();
	const auto& nextPos = *p2.pos.get();
	const auto& nextCp1 = *p2.cp1.get();
	auto t = (x - prevPos.getX()) / (nextPos.getX() - prevPos.getX());
	const float desiredTime = x;
	x = computeValueAtT(t, prevPos.getX(), prevPos.getX() + prevCp2.getX(), nextPos.getX() + nextCp1.getX(),
	                    nextPos.getX());
	float dist, last;

	float curError = std::numeric_limits<float>::max(), lastError = fabs((x / desiredTime) - 1);
	static const float kWalker = 0.5f;
	static const float kMaxError = 0.00025f;

	do
	{
		last = x;

		dist = (desiredTime - x) * kWalker;
		t += dist;

		x = computeValueAtT(t, prevPos.getX(), prevPos.getX() + prevCp2.getX(), nextPos.getX() + nextCp1.getX(),
		                    nextPos.getX());

		lastError = curError;
		if (desiredTime == 0.f)
		{
			curError = 0.f;
		}
		else
		{
			curError = fabs((x / desiredTime) - 1);
		}
	} while (curError < lastError && curError > kMaxError);

	return t;
}

void CubicBezierInterpolator::updateControlPoints(BezierPoint& point, BezierPoint* prevPoint, BezierPoint* nextPoint)
{
	if (prevPoint)
	{
		if (nextPoint)
		{
			auto t = timeAtX(point.pos->getX(), *prevPoint, *nextPoint);
			*prevPoint->cp2.get() *= t;
			*nextPoint->cp1.get() *= (1.f - t);
		}
		else
		{
			auto x = prevPoint->cp2->getX();
			auto maxx = (point.pos->getX() - prevPoint->pos->getX());
			// Don't allow the previous point's control point go beyond the new point
			if (x > 0 && x > maxx)
			{
				*prevPoint->cp2.get() *= (maxx / x);
			}
			else if (x < 0)
			{
				prevPoint->cp2->setX(maxx * 0.5f);
				prevPoint->cp2->setY(0.0f);
			}
		}
	}
	else if (nextPoint)
	{
		auto x = nextPoint->cp1->getX();
		auto minx = (point.pos->getX() - nextPoint->pos->getX());
		// Don't allow the next point's control point to precede the new point
		if (x < 0 && x < minx)
		{
			*nextPoint->cp1.get() *= (minx / x);
		}
		else if (x > 0)
		{
			prevPoint->cp2->setX(minx * 0.5f);
			prevPoint->cp2->setY(0.0f);
		}
	}
}
} // end namespace wgt
