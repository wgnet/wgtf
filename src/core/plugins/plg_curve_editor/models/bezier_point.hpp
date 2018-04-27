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

#include "core_object/managed_object.hpp"

namespace wgt
{
class Point;
struct BezierPointData;

class BezierPoint
{
public:
    BezierPoint(const std::string& id, const BezierPointData& data);

    BezierPointData getData() const;
    void setData(const BezierPointData& data);

	Point* pos() const;
	Point* cp1() const;
    Point* cp2() const;
    const std::string& id() const;

private:
    std::unique_ptr<Point> pos_;
	std::unique_ptr<Point> cp1_;
	std::unique_ptr<Point> cp2_;
    std::string id_;
};

bool operator==(const BezierPoint& lhs, const BezierPoint& rhs);
} // end namespace wgt
#endif // BEZIER_POINT_H_
