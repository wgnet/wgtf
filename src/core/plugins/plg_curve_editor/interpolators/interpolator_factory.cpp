//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  interpolator_factory.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "interpolator_factory.hpp"
#include "cubic_bezier_interpolator.hpp"
#include "linear_interpolator.hpp"

namespace wgt
{
ICurveInterpolatorPtr createInterpolator(CurveTypes::CurveType curveType)
{
	switch (curveType)
	{
	case CurveTypes::CubicBezier:
		return ICurveInterpolatorPtr(new CubicBezierInterpolator());
	case CurveTypes::Linear:
		return ICurveInterpolatorPtr(new LinearInterpolator());
	}
	return nullptr;
}
} // end namespace wgt
