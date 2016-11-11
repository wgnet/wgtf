//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  curve.hpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef CURVE_H_
#define CURVE_H_

#pragma once

#include "interfaces/i_curve.hpp"

#include <core_common/signal.hpp>
#include <core_dependency_system/i_interface.hpp>
#pragma warning(push)
#pragma warning(disable : 4996)
#include "core_data_model/generic_list.hpp"
#pragma warning(pop)

namespace wgt
{
class ICurveInterpolator;
class IDefinitionManager;
class BezierPoint;

typedef std::unique_ptr<ICurveInterpolator> ICurveInterpolatorPtr;

class Curve : public Implements<ICurve>
{
	typedef GenericListT<ObjectHandleT<BezierPoint>> TCurveList;
	typedef Signal<void(PointUpdateData)> PointSignal;
	typedef Signal<void(PointUpdateData, PointUpdateData)> ModifiedPointSignal;

public:
	Curve(ICurveInterpolatorPtr interpolator, IDefinitionManager* definitionManager);

	~Curve();

	/*! Adds a bezier point to the curve
	*/
	virtual void add(const BezierPointData&, bool triggerCallback) override;

	/*! Adds a new point at the specified time
	    @param time the exact point in time between 0.0 and 1.0 at which to add a point
	*/
	virtual void addAt(float time, bool triggerCallback) override;

	/*! Gets the value on the curve at the specified time
	    @param time the exact point in time between 0.0 and 1.0 at which calculate the value
	*/
	virtual float at(const float& time) override;

	/*! Gets the point data on the curve for a specific index
	*/
	virtual BezierPointData at(unsigned int index) override;

	/*! Enumerate the bezier points in this curve
	*/
	virtual void enumerate(PointCallback callback) override;

	/*! Gets the value determining if control points are shown
	*/
	virtual bool getShowControlPoints() const override
	{
		return showControlPoints_;
	}

	/*! Sets the value determining if control points are shown
	*/
	virtual void setShowControlPoints(const bool& showControlPoints) override
	{
		showControlPoints_ = showControlPoints;
	}

	/*! Removes the point at the specified time
	    @param time an exact time between 0.0 and 1.0 at which to remove a point
	*/
	virtual void removeAt(float time, bool triggerCallback) override;

	/*! Modifies the point at the specified index to be the given data
	    @param index The index of the point to modify
	    @param data The point data which will be set at the specified index
	*/
	virtual void modify(unsigned int index, const BezierPointData& data) override;

	/*! Registers a callback for when new points are added to the curve
	*/
	virtual void connectOnAdded(PointCallback callback) override
	{
		added_.connect(std::move(callback));
	}

	/*! Registers a callback for when points are removed from the curve
	*/
	virtual void connectOnRemoved(PointCallback callback) override
	{
		removed_.connect(std::move(callback));
	}

	/*! Registers a callback for when points are modified
	*/
	virtual void connectOnModified(ModifiedPointCallback callback) override
	{
		modified_.connect(callback);
	}

	virtual unsigned int getNumPoints() override
	{
		return (unsigned int)points_.size();
	}

	virtual ObjectHandleT<IValueChangeNotifier> curveDirty() const override
	{
		return ObjectHandleT<IValueChangeNotifier>(&dirty_);
	}

	/*! Redoes the last modification
	*/
	virtual void redo(const ObjectHandle& handle, Variant variant) override
	{
		modificationStack_[++currentState_].redo_();
	}

	/*! Undoes the last modification
	*/
	void undo(const ObjectHandle& handle, Variant variant) override
	{
		modificationStack_[currentState_--].undo_();
	}

protected:
	/*! Gets the collection of bezier points exposed through reflection
	*/
	virtual const IListModel* getPoints() const final
	{
		return &points_;
	}

private:
	typedef std::function<void()> ModificationFunction;
	struct CurveModification
	{
		CurveModification(ModificationFunction&& executeFunc, ModificationFunction&& undoFunc)
		    : undo_(std::move(undoFunc)), redo_(std::move(executeFunc))
		{
		}

		ModificationFunction undo_;
		ModificationFunction redo_;
	};

	void addListeners(ObjectHandleT<BezierPoint> bezierPoint);
	void removeListeners(ObjectHandleT<BezierPoint> bezierPoint);
	void insertPoint(ObjectHandleT<BezierPoint> bezierPoint, bool updateYPos, bool triggerCallback);
	void pushModification(ModificationFunction&& executeFunc, ModificationFunction&& undoFunc);
	unsigned int find_index(const BezierPointData& value);

	PointSignal added_;
	PointSignal removed_;
	ModifiedPointSignal modified_;
	TCurveList points_;
	std::vector<CurveModification> modificationStack_;
	size_t currentState_;
	IDefinitionManager* definitionManager_;

	bool showControlPoints_;
	ValueChangeNotifier<bool> dirty_;
	ICurveInterpolatorPtr interpolator_;
};
} // end namespace wgt
#endif // CURVE_H_
