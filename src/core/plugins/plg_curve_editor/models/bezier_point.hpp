//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  bezier_point.hpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef BEZIER_POINT_H_
#define BEZIER_POINT_H_

#pragma once

#include <core_reflection/object_handle.hpp>

namespace wgt
{
class Point;
class IDefinitionManager;
struct BezierPointData;

class BezierPoint
{
public:
	BezierPoint()
	{
	}
	BezierPoint(IDefinitionManager& definitionManager, const Point& pos);
	BezierPoint(IDefinitionManager& definitionManager, const Point& pos, const Point& cp1, const Point& cp2);

	BezierPoint& operator=(const BezierPointData& rhs);

	ObjectHandleT<Point> pos;
	ObjectHandleT<Point> cp1;
	ObjectHandleT<Point> cp2;
};

bool operator==(const BezierPoint& lhs, const BezierPoint& rhs);
} // end namespace wgt
#endif // BEZIER_POINT_H_
