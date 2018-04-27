//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  curve.hpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once

#include "curve_editor/i_curve.hpp"
#include "core_common/signal.hpp"
#include "core_dependency_system/i_interface.hpp"
#include "core_data_model/collection_model.hpp"
#include "core_object/managed_object.hpp"
#include "core_common/wg_read_write_lock.hpp"

namespace wgt
{
class ICurveInterpolator;
class BezierPoint;

typedef std::unique_ptr<ICurveInterpolator> ICurveInterpolatorPtr;

class Curve : public Implements<ICurve>
{
	typedef Signal<void(PointUpdateData)> PointSignal;
	typedef Signal<void(PointUpdateData, PointUpdateData)> ModifiedPointSignal;

public:
	Curve(ICurveInterpolatorPtr interpolator);

	~Curve();

	/*! Adds a bezier point to the curve
	*/
	virtual void add(const BezierPointData& data, bool triggerCallback) override;

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

	virtual bool getDirty() const override
	{
		return dirty_;
	}

	/*! Redoes the last modification
	*/
	virtual void redo(Variant params, Variant variant) override
	{
		modificationStack_[++currentState_].redo_();
	}

	/*! Undoes the last modification
	*/
	virtual void undo(Variant params, Variant variant) override
	{
		modificationStack_[currentState_--].undo_();
	}

	virtual void getDirtySignal(Signal<void(Variant&)>** result) const override
	{
		*result = const_cast<Signal<void(Variant&)>*>(&dirtySignal_);
	}

protected:
	/*! Gets the collection of bezier points exposed through reflection
	*/
	virtual const AbstractListModel* getPoints() const final
	{
		return &pointsModel_;
	}

private:
	Signal<void(Variant&)> dirtySignal_;

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
	void insertPoint(BezierPointData data, bool updateYPos, bool triggerCallback);
    void insertPoint(int index, ManagedObject<BezierPoint> bezierPoint, bool triggerCallback);
    void removePoint(int index, bool triggerCallback);
	void pushModification(ModificationFunction&& executeFunc, ModificationFunction&& undoFunc);
    unsigned int findIndex(const BezierPointData& value) const;
    unsigned int findIndex(const std::string& id) const;
    ObjectHandleT<BezierPoint> findPoint(const std::string& id) const;

    std::vector<ObjectHandleT<BezierPoint>> points_;
    std::vector<ManagedObject<BezierPoint>> pointObjects_;
	CollectionModel pointsModel_;
    mutable wg_read_write_lock pointsLock_;

    PointSignal added_;
    PointSignal removed_;
    ModifiedPointSignal modified_;
	std::vector<CurveModification> modificationStack_;
	size_t currentState_;
	bool showControlPoints_;
	bool dirty_;
	ICurveInterpolatorPtr interpolator_;
};
} // end namespace wgt