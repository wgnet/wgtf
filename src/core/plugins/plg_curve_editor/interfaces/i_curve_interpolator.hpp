//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  i_curve_interpolator.hpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef I_CURVE_INTERPOLATOR_H_
#define I_CURVE_INTERPOLATOR_H_

#pragma once

#include "models/bezier_point_data.hpp"
#include <memory>

namespace wgt
{
class BezierPoint;

class ICurveInterpolator
{
public:
	virtual ~ICurveInterpolator() {}

	/*! Finds the point on the curve between the specified points at the specified time
		@param time a value between 0.0 and 1.0 where 0.0 would be p1 and 1.0 would be p2
		@param p1 the starting point at time 0.0
		@param p2 the end point at time 1.0
	*/
	virtual BezierPointData interpolate(float time, const BezierPoint& p1, const BezierPoint& p2) = 0;

	/*! Finds the time between 0.0 and 1.0 along the curve given the specified value x.
		Can iterate until the value is within an acceptable error threshold.
	*/
	virtual float timeAtX(float x, const BezierPoint& p1, const BezierPoint& p2) = 0;

	/*! Update the control points for the previous and next positions of the given point
	*/
	virtual void updateControlPoints(BezierPoint& point, BezierPoint* prevPoint, BezierPoint* nextPoint) = 0;
};

typedef std::unique_ptr<ICurveInterpolator> ICurveInterpolatorPtr;
} // end namespace wgt
#endif // I_CURVE_INTERPOLATOR_H_
