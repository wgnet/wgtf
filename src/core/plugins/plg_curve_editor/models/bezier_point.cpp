//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  bezier_point.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "bezier_point.hpp"
#include "curve_editor/bezier_point_data.hpp"
#include "curve_editor/curve_types.hpp"
#include "point.hpp"

namespace wgt
{
const std::string& BezierPoint::id() const
{
    return id_;
}

BezierPoint::BezierPoint(const std::string& id, const BezierPointData& data)
    : id_(id)
{
    pos_ = std::make_unique<Point>();
    cp1_ = std::make_unique<Point>();
    cp2_ = std::make_unique<Point>();
    setData(data);
}

void BezierPoint::setData(const BezierPointData& data)
{
    pos_->setX(data.pos.x);
    pos_->setY(data.pos.y);
    cp1_->setX(data.cp1.x);
    cp1_->setY(data.cp1.y);
    cp2_->setX(data.cp2.x);
    cp2_->setY(data.cp2.y);
}

BezierPointData BezierPoint::getData() const
{
    BezierPointData data;
    data.pos.x = pos_->getX();
    data.pos.y = pos_->getY();
    data.cp1.x = cp1_->getX();
    data.cp1.y = cp1_->getY();
    data.cp2.x = cp2_->getX();
    data.cp2.y = cp2_->getY();
    return data;
}

Point* BezierPoint::pos() const
{
    return pos_.get();
}

Point* BezierPoint::cp1() const
{
    return cp1_.get();
}

Point* BezierPoint::cp2() const
{
    return cp2_.get();
}

bool operator==(const BezierPoint& lhs, const BezierPoint& rhs)
{
	return lhs.pos() == rhs.pos() && lhs.cp1() == rhs.cp1() && lhs.cp2() == rhs.cp2();
}
} // end namespace wgt
