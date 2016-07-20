//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  curve.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "curve.hpp"
#include "models/point.hpp"
#include "models/bezier_point.hpp"
#include "interfaces/i_curve_interpolator.hpp"

#include <core_data_model/i_item_role.hpp>
#include <core_reflection/object_handle.hpp>
#include <core_reflection/definition_manager.hpp>
#include <core_reflection/type_class_definition.hpp>

namespace wgt
{
BezierPointData getPointData( const BezierPoint& point )
{
	BezierPointData data = {
		{ point.pos->getX(), point.pos->getY() },
		{ point.cp1->getX(), point.cp1->getY() },
		{ point.cp2->getX(), point.cp2->getY() }
	};
	return data;
}

void setPointData( const BezierPointData& data, BezierPoint& point )
{
	point.pos->setX(data.pos.x);
	point.pos->setY(data.pos.y);
	point.cp1->setX(data.cp1.x);
	point.cp1->setY(data.cp1.y);
	point.cp2->setX(data.cp2.x);
	point.cp2->setY(data.cp2.y);
}

Curve::Curve(ICurveInterpolatorPtr interpolator, IDefinitionManager* definitionManager)
	: currentState_(-1)
	, definitionManager_(definitionManager)
	, showControlPoints_(false)
	, dirty_(false)
	, interpolator_(std::move(interpolator))
{
}

Curve::~Curve()
{
}

void Curve::add(const BezierPointData& point, bool triggerCallback)
{
	if ( definitionManager_ == nullptr )
	{
		assert(!"Failed to add point because definition manager is null");
		return;
	}

	auto def = definitionManager_->getDefinition<BezierPoint>();
	if ( def == nullptr )
	{
		assert(!"Failed to add point because BezierPoint definition is null");
		return;
	}

	auto handle = TypeClassDefinition<BezierPoint>::create(*def, *definitionManager_,
		Point(point.pos.x, point.pos.y), Point(point.cp1.x, point.cp1.y), Point(point.cp2.x, point.cp2.y));
	
	insertPoint(handle, false, triggerCallback);
}

void Curve::addAt(float x, bool triggerCallback)
{
	if ( definitionManager_ == nullptr )
		return;

	auto ptDefinition = definitionManager_->getDefinition<Point>();
	if ( ptDefinition == nullptr )
		return;
	
	auto bezPtDefinition = definitionManager_->getDefinition<BezierPoint>();
	auto newBezPointHandle = TypeClassDefinition<BezierPoint>::create(
		*bezPtDefinition, *definitionManager_, Point(x, 0));

	insertPoint( newBezPointHandle, true, triggerCallback );
}

float Curve::at(const float& time)
{
	BezierPoint* prevPoint = nullptr;
	for ( auto iter = std::begin(points_); iter != std::end(points_); ++iter )
	{
		auto& point = *(*iter);
		if ( point.pos->getX() >= time )
		{
			if ( prevPoint )
			{
				auto localTime = interpolator_->timeAtX(time, *prevPoint, point);
				return interpolator_->interpolate(localTime, *prevPoint, point).pos.y;
			}
			else
			{
				return point.pos->getY();
			}
			break;
		}
		prevPoint = &point;
	}

	return prevPoint ? prevPoint->pos->getY() : 0.0f;
}

BezierPointData Curve::at(unsigned int index)
{
	assert(index < points_.size());
	auto& point = *points_[index]; 

	BezierPointData pointData = {
		{ point.pos->getX(), point.pos->getY() },
		{ point.cp1->getX(), point.cp1->getY() },
		{ point.cp2->getX(), point.cp2->getY() }
	};

	return pointData;
}

void Curve::enumerate(PointCallback callback)
{
	auto index = 0;
	for( auto iter = points_.begin(); iter != points_.end(); ++iter, ++index )
	{
		auto& point = *(*iter);

		BezierPointData data = {
			{ point.pos->getX(), point.pos->getY() },
			{ point.cp1->getX(), point.cp1->getY() },
			{ point.cp2->getX(), point.cp2->getY() }
		};
		PointUpdateData current;
		current.point = data;
		current.index = index;
		callback(current);
	}
}

void Curve::removeAt(float time, bool triggerCallback)
{
	ObjectHandleT<BezierPoint> bezierPoint;
	auto index = 0;
	for ( auto iter = std::begin(points_); iter != std::end(points_); ++iter, ++index )
	{
		auto& point = *( *iter );
		if ( point.pos->getX() == time )
		{
			bezierPoint = *iter;
			break;
		}
	}

	if(bezierPoint.get() == nullptr)
		return;

	auto& points = points_;
	auto& removedCallback = removed_;
	auto removeListenersFunc = [this](ObjectHandleT<BezierPoint> bezierPoint){ removeListeners(bezierPoint); };
	auto executeFunc = [bezierPoint, removeListenersFunc, triggerCallback, &points, &removedCallback]()
	{
		auto index = 0;
		for( auto iter = std::begin(points); iter != std::end(points); ++iter, ++index )
		{
			auto& point = *(*iter);
			if ( &point == bezierPoint.get() )
			{
				removeListenersFunc(*iter);
				points.erase(iter);
				if(triggerCallback)
				{
					BezierPointData data = {
						{ point.pos->getX(), point.pos->getY() },
						{ point.cp1->getX(), point.cp1->getY() },
						{ point.cp2->getX(), point.cp2->getY() }
					};
					PointUpdateData removed;
					removed.point = data;
					removed.index = index;
					removedCallback(removed);
				}
				break;
			}
		}
	};

	auto& addedCallback = added_;
	auto addListenersFunc = [this](ObjectHandleT<BezierPoint> bezierPoint){ addListeners(bezierPoint); };
	auto undoFunc = [index, bezierPoint, addListenersFunc, triggerCallback, &points, &addedCallback]()
	{
		auto currentIndex = 0;
		auto iter = std::begin(points);
		for ( ; iter != std::end(points); ++iter, ++currentIndex )
		{
			if(currentIndex == index)
				break;
		}

		points.insert(iter, bezierPoint);

		addListenersFunc(bezierPoint);

		if ( triggerCallback )
		{
			BezierPointData data = {
				{ bezierPoint->pos->getX(), bezierPoint->pos->getY() },
				{ bezierPoint->cp1->getX(), bezierPoint->cp1->getY() },
				{ bezierPoint->cp2->getX(), bezierPoint->cp2->getY() }
			};
			PointUpdateData added;
			added.point = data;
			added.index = index;
			addedCallback(added);
		}
	};

	pushModification(std::move(executeFunc), std::move(undoFunc));
}

void Curve::modify(unsigned int index, const BezierPointData& data)
{
	if( index >= points_.size() )
		return;

	BezierPoint& point = *points_[index];
	point.pos->setX(data.pos.x);
	point.pos->setY(data.pos.y);
	point.cp1->setX(data.cp1.x);
	point.cp1->setY(data.cp1.y);
	point.cp2->setX(data.cp2.x);
	point.cp2->setY(data.cp2.y);
	dirty_.value(true);
	dirty_.value(false);
	
}

unsigned int Curve::find_index(const BezierPointData& value)
{
	unsigned int index = 0;
	for( auto iter = std::begin(points_); iter != std::end(points_); ++iter, ++index )
	{
		auto& point = *(*iter);
		if( point.pos->getX() == value.pos.x )
			return index;
	}

	return -1;
}

void Curve::addListeners(ObjectHandleT<BezierPoint> bezierPoint)
{
	bezierPoint->pos->xChanged.connect( [=](float oldX, float newX){
		BezierPointData oldData = {
			{ oldX, bezierPoint->pos->getY() },
			{ bezierPoint->cp1->getX(), bezierPoint->cp1->getY() },
			{ bezierPoint->cp2->getX(), bezierPoint->cp2->getY() }
		};
		PointUpdateData oldPoint;
		oldPoint.point = oldData;

		BezierPointData newData = {
			{ newX, bezierPoint->pos->getY() },
			{ bezierPoint->cp1->getX(), bezierPoint->cp1->getY() },
			{ bezierPoint->cp2->getX(), bezierPoint->cp2->getY() }
		};
		PointUpdateData newPoint;
		newPoint.point = newData;

		oldPoint.index = find_index(newPoint.point);
		newPoint.index = oldPoint.index;

		modified_(oldPoint, newPoint);
	} );

	bezierPoint->pos->yChanged.connect( [=](float oldY, float newY){
		BezierPointData oldData = {
			{ bezierPoint->pos->getX(), oldY },
			{ bezierPoint->cp1->getX(), bezierPoint->cp1->getY() },
			{ bezierPoint->cp2->getX(), bezierPoint->cp2->getY() }
		};
		PointUpdateData oldPoint;
		oldPoint.point = oldData;
		oldPoint.index = find_index(oldPoint.point);

		BezierPointData newData = {
			{ bezierPoint->pos->getX(), newY },
			{ bezierPoint->cp1->getX(), bezierPoint->cp1->getY() },
			{ bezierPoint->cp2->getX(), bezierPoint->cp2->getY() }
		};
		PointUpdateData newPoint;
		newPoint.point = newData;
		newPoint.index = oldPoint.index;

		modified_(oldPoint, newPoint);
	} );

	bezierPoint->cp1->xChanged.connect( [=](float oldX, float newX){
		BezierPointData oldData = {
			{ bezierPoint->pos->getX(), bezierPoint->pos->getY() },
			{ oldX, bezierPoint->cp1->getY() },
			{ bezierPoint->cp2->getX(), bezierPoint->cp2->getY() }
		};
		PointUpdateData oldPoint;
		oldPoint.point = oldData;
		oldPoint.index = find_index(oldPoint.point);

		BezierPointData newData = {
			{ bezierPoint->pos->getX(), bezierPoint->pos->getY() },
			{ newX, bezierPoint->cp1->getY() },
			{ bezierPoint->cp2->getX(), bezierPoint->cp2->getY() }
		};
		PointUpdateData newPoint;
		newPoint.point = newData;
		newPoint.index = oldPoint.index;

		modified_(oldPoint, newPoint);
	} );

	bezierPoint->cp1->yChanged.connect( [=](float oldY, float newY){
		BezierPointData oldData = {
			{ bezierPoint->pos->getX(), bezierPoint->pos->getY() },
			{ bezierPoint->cp1->getX(), oldY },
			{ bezierPoint->cp2->getX(), bezierPoint->cp2->getY() }
		};
		PointUpdateData oldPoint;
		oldPoint.point = oldData;
		oldPoint.index = find_index(oldPoint.point);

		BezierPointData newData = {
			{ bezierPoint->pos->getX(), bezierPoint->pos->getY() },
			{ bezierPoint->cp1->getX(), newY },
			{ bezierPoint->cp2->getX(), bezierPoint->cp2->getY() }
		};
		PointUpdateData newPoint;
		newPoint.point = newData;
		newPoint.index = oldPoint.index;

		modified_(oldPoint, newPoint);
	} );

	bezierPoint->cp2->xChanged.connect( [=](float oldX, float newX){
		BezierPointData oldData = {
			{ bezierPoint->pos->getX(), bezierPoint->pos->getY() },
			{ bezierPoint->cp1->getX(), bezierPoint->cp1->getY() },
			{ oldX, bezierPoint->cp2->getY() },
		};
		PointUpdateData oldPoint;
		oldPoint.point = oldData;
		oldPoint.index = find_index(oldPoint.point);

		BezierPointData newData = {
			{ bezierPoint->pos->getX(), bezierPoint->pos->getY() },
			{ bezierPoint->cp1->getX(), bezierPoint->cp1->getY() },
			{ newX, bezierPoint->cp2->getY() },
		};
		PointUpdateData newPoint;
		newPoint.point = newData;
		newPoint.index = oldPoint.index;

		modified_(oldPoint, newPoint);
	} );

	bezierPoint->cp2->yChanged.connect( [=](float oldY, float newY){
		BezierPointData oldData = {
			{ bezierPoint->pos->getX(), bezierPoint->pos->getY() },
			{ bezierPoint->cp1->getX(), bezierPoint->cp1->getY() },
			{ bezierPoint->cp2->getX(), oldY },
		};
		PointUpdateData oldPoint;
		oldPoint.point = oldData;
		oldPoint.index = find_index(oldPoint.point);

		BezierPointData newData = {
			{ bezierPoint->pos->getX(), bezierPoint->pos->getY() },
			{ bezierPoint->cp1->getX(), bezierPoint->cp1->getY() },
			{ bezierPoint->cp2->getX(), newY },
		};
		PointUpdateData newPoint;
		newPoint.point = newData;
		newPoint.index = oldPoint.index;

		modified_(oldPoint, newPoint);
	} );
}

void Curve::removeListeners(ObjectHandleT<BezierPoint> bezierPoint)
{
	bezierPoint->pos->xChanged.clear();
	bezierPoint->pos->yChanged.clear();
	bezierPoint->cp1->xChanged.clear();
	bezierPoint->cp1->yChanged.clear();
	bezierPoint->cp2->xChanged.clear();
	bezierPoint->cp2->yChanged.clear();
}

void Curve::insertPoint(ObjectHandleT<BezierPoint> bezierPoint, bool updateYPos, bool triggerCallback)
{
	auto& newPoint = *bezierPoint.get();
	auto& newPos = *bezierPoint->pos.get();
	BezierPoint* prevPoint = nullptr;
	BezierPoint* nextPoint = nullptr;

	auto x = newPoint.pos->getX();

	auto iter = std::begin(points_);
	for ( ; iter != std::end(points_); ++iter )
	{
		nextPoint = &*( *iter );
		if ( nextPoint->pos->getX() > x )
		{
			if(updateYPos)
			{
				newPos.setY(nextPoint->pos->getY());
			}
			break;
		}
		prevPoint = nextPoint;
	}

	if ( iter == std::end(points_) )
	{
		nextPoint = nullptr;
	}

	auto interpolator = interpolator_.get();
	auto& points = points_;
	auto& addCallback = added_;
	auto addListenersFunc = [this](ObjectHandleT<BezierPoint> bezierPoint){ addListeners(bezierPoint); };
	auto executeFunc = [bezierPoint, prevPoint, nextPoint, interpolator,
		x, updateYPos, triggerCallback, addListenersFunc, &points, &addCallback]()
	{
		auto& newPoint = *bezierPoint.get();
		auto& newPos = *bezierPoint->pos.get();

		size_t index = 0;
		auto iter = std::begin(points);
		for ( ; iter != std::end(points); ++iter )
		{
			if ( (*iter)->pos->getX() > x )
				break;
			++index;
		}

		// Adding the point to the end of the curve
		if ( nextPoint == nullptr )
		{
			if( updateYPos )
			{
				newPos.setY(prevPoint ? prevPoint->pos->getY() : 0);
			}
			if ( prevPoint )
			{
				interpolator->updateControlPoints(newPoint, prevPoint, nullptr);
			}
			index = points.size();
			points.push_back(bezierPoint);
		}
		else
		{
			// Are there both previous and next points?
			if ( prevPoint )
			{
				auto t = interpolator->timeAtX(x, *prevPoint, *nextPoint);
				if( updateYPos )
				{
					newPoint = interpolator->interpolate(t, *prevPoint, *nextPoint);
				}
				interpolator->updateControlPoints(newPoint, prevPoint, nextPoint);
			}
			else
			{
				// There is only a next point, update the control point
				interpolator->updateControlPoints(newPoint, prevPoint, nextPoint);
			}
			points.insert(iter, std::move(bezierPoint));
		}

		// Listen to point modifications
		addListenersFunc(bezierPoint);

		// Notify the point was added to the curve
		if( triggerCallback )
		{
			BezierPointData point = {
				{ bezierPoint->pos->getX(), bezierPoint->pos->getY() },
				{ bezierPoint->cp1->getX(), bezierPoint->cp1->getY() },
				{ bezierPoint->cp2->getX(), bezierPoint->cp2->getY() }
			};
			PointUpdateData data;
			data.point = point;
			data.index = (unsigned int)index;

			addCallback(data);
		}
	};
	
	auto nextData = nextPoint ? getPointData(*nextPoint) : BezierPointData();
	auto prevData = prevPoint ? getPointData(*prevPoint) : BezierPointData();
	auto& removedCallback = removed_;
	auto removeListenersFunc = [this](ObjectHandleT<BezierPoint> bezierPoint){ removeListeners(bezierPoint); };
	auto undoFunc = [bezierPoint, nextData, prevData, nextPoint, prevPoint,
		triggerCallback, removeListenersFunc, &points, &removedCallback](){

		removeListenersFunc(bezierPoint);

		if(nextPoint)
		{
			setPointData(nextData, *nextPoint);
		}
		if(prevPoint)
		{
			setPointData(prevData, *prevPoint);
		}
		
		auto index = 0;
		for ( auto iter = std::begin(points); iter != std::end(points); ++iter, ++index )
		{
			if(*iter == bezierPoint)
			{
				points.erase(iter);
				break;
			}
		}
		if(triggerCallback)
		{
			BezierPointData point = {
				{ bezierPoint->pos->getX(), bezierPoint->pos->getY() },
				{ bezierPoint->cp1->getX(), bezierPoint->cp1->getY() },
				{ bezierPoint->cp2->getX(), bezierPoint->cp2->getY() }
			};
			PointUpdateData removed;
			removed.point = point;
			removed.index = index;
			removedCallback(removed);
		}
	};

	pushModification(std::move(executeFunc), std::move(undoFunc));
}

void Curve::pushModification(ModificationFunction&& executeFunc, ModificationFunction&& undoFunc)
{
	while ( currentState_ < modificationStack_.size() - 1 )
	{
		modificationStack_.pop_back();
	}
	++currentState_;
	executeFunc();
	modificationStack_.emplace_back(std::move(executeFunc), std::move(undoFunc));
}
} // end namespace wgt
