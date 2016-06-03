//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  bezier_point.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "bezier_point.hpp"
#include "bezier_point_data.hpp"
#include "point.hpp"

#include <core_reflection/type_class_definition.hpp>

namespace wgt
{
BezierPoint::BezierPoint(IDefinitionManager& definitionManager, const Point& pos)
{
	auto def = definitionManager.getDefinition<Point>();
	if(def)
	{
		this->pos = TypeClassDefinition<Point>::create(*def, pos);
		cp1 = safeCast<Point>(def->create());
		cp2 = safeCast<Point>(def->create());
	}
}

BezierPoint::BezierPoint(IDefinitionManager& definitionManager, const Point& pos, const Point& cp1, const Point& cp2)
{
	auto def = definitionManager.getDefinition<Point>();
	if ( def )
	{
		this->pos = TypeClassDefinition<Point>::create(*def, pos);
		this->cp1 = TypeClassDefinition<Point>::create(*def, cp1);
		this->cp2 = TypeClassDefinition<Point>::create(*def, cp2);
	}
}

BezierPoint& BezierPoint::operator=( const BezierPointData& rhs )
{
	pos->setX(rhs.pos.x);
	pos->setY(rhs.pos.y);
	cp1->setX(rhs.cp1.x);
	cp1->setY(rhs.cp1.y);
	cp2->setX(rhs.cp2.x);
	cp2->setY(rhs.cp2.y);
	return *this;
}


bool operator==( BezierPoint& lhs, BezierPoint& rhs )
{
	return lhs.pos == rhs.pos
		&& lhs.cp1 == rhs.cp1
		&& lhs.cp2 == rhs.cp2;
}
} // end namespace wgt
