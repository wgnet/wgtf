//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  i_curve.hpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef I_CURVE_H_
#define I_CURVE_H_

#pragma once

#include "../models/bezier_point_data.hpp"

#include <assert.h>
#include <core_reflection/reflected_object.hpp>
#include <core_reflection/object_handle.hpp>
#include <core_data_model/i_list_model.hpp>
#include <core_data_model/value_change_notifier.hpp>
#include <wg_types/vector2.hpp>

namespace wgt
{
struct PointUpdateData
{
	unsigned int index;
	BezierPointData point;
};

/*!
 * \class ICurve
 *
 * \brief Interface used with the curve editor allowing modification of the curve and
 * the ability to receive notifications when the curve is modified
 *
 * \author m_martin
 * \date August 2015
 */
class ICurve
{
	DECLARE_REFLECTED;

public:
	virtual ~ICurve()
	{
	}

	typedef std::function<void(PointUpdateData)> PointCallback;
	typedef std::function<void(PointUpdateData, PointUpdateData)> ModifiedPointCallback;

	/*! Adds a bezier point to the curve
	*/
	virtual void add(const BezierPointData&, bool triggerCallback = true) = 0;

	/*! Adds a new point at the specified time
	    @param time the exact point in time between 0.0 and 1.0 at which to add a point
	*/
	virtual void addAt(float /*time*/, bool triggerCallback) = 0;

	/*! Gets the value on the curve at the specified time
	    @param time the exact point in time between 0.0 and 1.0 at which calculate the value
	*/
	virtual float at(const float& /*time*/) = 0;

	/*! Gets the point data on the curve for a specific index
	*/
	virtual BezierPointData at(unsigned int /*index*/) = 0;

	/*! Removes the point at the specified time
	    @param time an exact time between 0.0 and 1.0 at which to remove a point
	*/
	virtual void removeAt(float /*time*/, bool triggerCallback) = 0;

	/*! Modifies the point at the specified index to be the given data
	    @param index The index of the point to modify
	    @param data The point data which will be set at the specified index
	*/
	virtual void modify(unsigned int /*index*/, const BezierPointData& data) = 0;

	/*! Enumerate the bezier points in this curve
	*/
	virtual void enumerate(PointCallback) = 0;

	/*! Registers a callback for when new points are added to the curve
	*/
	virtual void connectOnAdded(PointCallback) = 0;

	/*! Registers a callback for when points are removed from the curve
	*/
	virtual void connectOnRemoved(PointCallback) = 0;

	/*! Registers a callback for when points are modified
	*/
	virtual void connectOnModified(ModifiedPointCallback) = 0;

	/*! Gets the value determining if control points are shown
	*/
	virtual bool getShowControlPoints() const = 0;

	/*! Sets the value determining if control points are shown
	*/
	virtual void setShowControlPoints(const bool&) = 0;

	virtual unsigned int getNumPoints() = 0;

	virtual ObjectHandleT<IValueChangeNotifier> curveDirty() const = 0;

	/*! Redoes the last modification
	*/
	virtual void redo(const ObjectHandle& handle, Variant variant) = 0;

	/*! Undoes the last modification
	*/
	virtual void undo(const ObjectHandle& handle, Variant variant) = 0;

protected:
	/*! Gets the collection of bezier points exposed through reflection
	*/
	virtual const IListModel* getPoints() const = 0;

private:
	/*! Calls at() to get the value on the curve at the specified time.
	    (Can't expose overloaded functions to QML)
	*/
	float atTime(float time)
	{
		return at(time);
	}

	/*! Calls at() to get the value on the curve at the specified index.
	    (Can't expose overloaded functions to QML)
	*/
	Vector2 atIndex(unsigned int index)
	{
		auto data = at(index);
		return Vector2(data.pos.x, data.pos.y);
	}
};
} // end namespace wgt
#endif // I_CURVE_H_
