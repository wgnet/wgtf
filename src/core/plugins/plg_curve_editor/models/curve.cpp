//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  curve.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "curve.hpp"
#include "core_dependency_system/depends.hpp"
#include "models/point.hpp"
#include "models/bezier_point.hpp"
#include "curve_editor/i_curve_interpolator.hpp"

#include <core_common/assert.hpp>
#include <core_data_model/i_item_role.hpp>
#include <core_reflection/object_handle.hpp>
#include <core_reflection/definition_manager.hpp>
#include <core_reflection/type_class_definition.hpp>
#include "core_logging/logging.hpp"

namespace wgt
{
static const float EPSILON = 0.0005f;

Curve::Curve(ICurveInterpolatorPtr interpolator)
    : currentState_(-1)
    , showControlPoints_(false), dirty_(false)
    , interpolator_(std::move(interpolator))
{
	pointsModel_.setSource(Collection(points_));
}

Curve::~Curve()
{
    wg_read_lock_guard guard(pointsLock_);
	for (auto& point : points_)
    {
		removeListeners(point);
	}
}

void Curve::add(const BezierPointData& data, bool triggerCallback)
{
	insertPoint(data, false, triggerCallback);
}

void Curve::addAt(float x, bool triggerCallback)
{
    BezierPointData data = {};
    data.pos.x = x;
	insertPoint(data, true, triggerCallback);
}

float Curve::at(const float& time)
{
    wg_read_lock_guard guard(pointsLock_);

	BezierPoint* prevPoint = nullptr;
	for (auto iter = std::begin(points_); iter != std::end(points_); ++iter)
	{
		auto& point = *(*iter);
		if (point.pos()->getX() >= time)
		{
			if (prevPoint)
			{
				auto localTime = interpolator_->timeAtX(time, *prevPoint, point);
				return interpolator_->interpolate(localTime, *prevPoint, point).pos.y;
			}
			else
			{
				return point.pos()->getY();
			}
			break;
		}
		prevPoint = &point;
	}

	return prevPoint ? prevPoint->pos()->getY() : 0.0f;
}

BezierPointData Curve::at(unsigned int index)
{
    wg_read_lock_guard guard(pointsLock_);
	TF_ASSERT(index < points_.size());
    return points_[index]->getData();
}

void Curve::enumerate(PointCallback callback)
{
    wg_read_lock_guard guard(pointsLock_);
	auto index = 0;
	for (auto iter = points_.begin(); iter != points_.end(); ++iter, ++index)
	{
		PointUpdateData current;
		current.point = (*iter)->getData();
		current.index = index;
		callback(current);
	}
}

void Curve::removeAt(float time, bool triggerCallback)
{
    ObjectHandleT<BezierPoint> bezierPoint;
    auto index = 0;
    {
        wg_read_lock_guard guard(pointsLock_);
        for (auto iter = std::begin(points_); iter != std::end(points_); ++iter, ++index)
        {
            if (abs((*iter)->pos()->getX() - time) < EPSILON)
            {
                bezierPoint = *iter;
                break;
            }
        }
    }

	if (bezierPoint == nullptr)
	{
		NGT_ERROR_MSG("Failed to find point at time %lf\n", time);
		pushModification([]() {}, []() {});
		return;
	}

    // Object handles may not last the lifetime of the callbacks, do not store them in lambda
	auto executeFunc = [id = bezierPoint->id(), triggerCallback, index, this]()
    {
        TF_ASSERT(findIndex(id) == index);
        removePoint(index, triggerCallback);
        TF_ASSERT(findIndex(id) == -1);
	};

    // Object handles may not last the lifetime of the callbacks, do not store them in lambda
	auto undoFunc = [data = bezierPoint->getData(), id = bezierPoint->id(), index, triggerCallback, this]()
    {
        TF_ASSERT(findIndex(id) == -1);
		auto newPointStorage = TypeClassDefinition<BezierPoint>::createObjectStorage(id, data);
        insertPoint(index, ManagedObject<BezierPoint>(newPointStorage, id), triggerCallback);
        TF_ASSERT(findIndex(id) == index);
	};

	pushModification(std::move(executeFunc), std::move(undoFunc));
}

void Curve::modify(unsigned int index, const BezierPointData& data)
{
    wg_read_lock_guard guard(pointsLock_);

	if (index >= points_.size())
		return;

    points_[index]->setData(data);

	dirty_ = true;
	Variant dirtyVariant = dirty_;
	dirtySignal_(dirtyVariant);
	dirty_ = false;
	dirtyVariant = dirty_;
	dirtySignal_(dirtyVariant);
}

ObjectHandleT<BezierPoint> Curve::findPoint(const std::string& id) const
{
    wg_read_lock_guard guard(pointsLock_);

    auto index = 0;
    for (auto iter = std::begin(points_); iter != std::end(points_); ++iter, ++index)
    {
        if ((*iter)->id() == id)
        {
            return *iter;
        }
    }
    return nullptr;
}

unsigned int Curve::findIndex(const std::string& id) const
{
    wg_read_lock_guard guard(pointsLock_);

    auto index = 0;
    for (auto iter = std::begin(points_); iter != std::end(points_); ++iter, ++index)
    {
        if ((*iter)->id() == id)
        {
            return index;
        }
    }
    return -1;
}

unsigned int Curve::findIndex(const BezierPointData& value) const
{
    wg_read_lock_guard guard(pointsLock_);

	unsigned int index = 0;
	for (auto iter = std::begin(points_); iter != std::end(points_); ++iter, ++index)
	{
		auto& point = *(*iter);
		if (point.pos()->getX() == value.pos.x)
			return index;
	}
	return -1;
}

void Curve::addListeners(ObjectHandleT<BezierPoint> bezierPoint)
{
	bezierPoint->pos()->xChanged.connect([=](float oldX, float newX) {
		BezierPointData oldData = { { oldX, bezierPoint->pos()->getY() },
			                        { bezierPoint->cp1()->getX(), bezierPoint->cp1()->getY() },
			                        { bezierPoint->cp2()->getX(), bezierPoint->cp2()->getY() } };
		PointUpdateData oldPoint;
		oldPoint.point = oldData;

		BezierPointData newData = { { newX, bezierPoint->pos()->getY() },
			                        { bezierPoint->cp1()->getX(), bezierPoint->cp1()->getY() },
			                        { bezierPoint->cp2()->getX(), bezierPoint->cp2()->getY() } };
		PointUpdateData newPoint;
		newPoint.point = newData;

		oldPoint.index = findIndex(newPoint.point);
		newPoint.index = oldPoint.index;

		modified_(oldPoint, newPoint);
	});

	bezierPoint->pos()->yChanged.connect([=](float oldY, float newY) {
		BezierPointData oldData = { { bezierPoint->pos()->getX(), oldY },
			                        { bezierPoint->cp1()->getX(), bezierPoint->cp1()->getY() },
			                        { bezierPoint->cp2()->getX(), bezierPoint->cp2()->getY() } };
		PointUpdateData oldPoint;
		oldPoint.point = oldData;
		oldPoint.index = findIndex(oldPoint.point);

		BezierPointData newData = { { bezierPoint->pos()->getX(), newY },
			                        { bezierPoint->cp1()->getX(), bezierPoint->cp1()->getY() },
			                        { bezierPoint->cp2()->getX(), bezierPoint->cp2()->getY() } };
		PointUpdateData newPoint;
		newPoint.point = newData;
		newPoint.index = oldPoint.index;

		modified_(oldPoint, newPoint);
	});

	bezierPoint->cp1()->xChanged.connect([=](float oldX, float newX) {
		BezierPointData oldData = { { bezierPoint->pos()->getX(), bezierPoint->pos()->getY() },
			                        { oldX, bezierPoint->cp1()->getY() },
			                        { bezierPoint->cp2()->getX(), bezierPoint->cp2()->getY() } };
		PointUpdateData oldPoint;
		oldPoint.point = oldData;
		oldPoint.index = findIndex(oldPoint.point);

		BezierPointData newData = { { bezierPoint->pos()->getX(), bezierPoint->pos()->getY() },
			                        { newX, bezierPoint->cp1()->getY() },
			                        { bezierPoint->cp2()->getX(), bezierPoint->cp2()->getY() } };
		PointUpdateData newPoint;
		newPoint.point = newData;
		newPoint.index = oldPoint.index;

		modified_(oldPoint, newPoint);
	});

	bezierPoint->cp1()->yChanged.connect([=](float oldY, float newY) {
		BezierPointData oldData = { { bezierPoint->pos()->getX(), bezierPoint->pos()->getY() },
			                        { bezierPoint->cp1()->getX(), oldY },
			                        { bezierPoint->cp2()->getX(), bezierPoint->cp2()->getY() } };
		PointUpdateData oldPoint;
		oldPoint.point = oldData;
		oldPoint.index = findIndex(oldPoint.point);

		BezierPointData newData = { { bezierPoint->pos()->getX(), bezierPoint->pos()->getY() },
			                        { bezierPoint->cp1()->getX(), newY },
			                        { bezierPoint->cp2()->getX(), bezierPoint->cp2()->getY() } };
		PointUpdateData newPoint;
		newPoint.point = newData;
		newPoint.index = oldPoint.index;

		modified_(oldPoint, newPoint);
	});

	bezierPoint->cp2()->xChanged.connect([=](float oldX, float newX) {
		BezierPointData oldData = {
			{ bezierPoint->pos()->getX(), bezierPoint->pos()->getY() },
			{ bezierPoint->cp1()->getX(), bezierPoint->cp1()->getY() },
			{ oldX, bezierPoint->cp2()->getY() },
		};
		PointUpdateData oldPoint;
		oldPoint.point = oldData;
		oldPoint.index = findIndex(oldPoint.point);

		BezierPointData newData = {
			{ bezierPoint->pos()->getX(), bezierPoint->pos()->getY() },
			{ bezierPoint->cp1()->getX(), bezierPoint->cp1()->getY() },
			{ newX, bezierPoint->cp2()->getY() },
		};
		PointUpdateData newPoint;
		newPoint.point = newData;
		newPoint.index = oldPoint.index;

		modified_(oldPoint, newPoint);
	});

	bezierPoint->cp2()->yChanged.connect([=](float oldY, float newY) {
		BezierPointData oldData = {
			{ bezierPoint->pos()->getX(), bezierPoint->pos()->getY() },
			{ bezierPoint->cp1()->getX(), bezierPoint->cp1()->getY() },
			{ bezierPoint->cp2()->getX(), oldY },
		};
		PointUpdateData oldPoint;
		oldPoint.point = oldData;
		oldPoint.index = findIndex(oldPoint.point);

		BezierPointData newData = {
			{ bezierPoint->pos()->getX(), bezierPoint->pos()->getY() },
			{ bezierPoint->cp1()->getX(), bezierPoint->cp1()->getY() },
			{ bezierPoint->cp2()->getX(), newY },
		};
		PointUpdateData newPoint;
		newPoint.point = newData;
		newPoint.index = oldPoint.index;

		modified_(oldPoint, newPoint);
	});
}

void Curve::removeListeners(ObjectHandleT<BezierPoint> bezierPoint)
{
    bezierPoint->pos()->xChanged.clear();
    bezierPoint->pos()->yChanged.clear();
    bezierPoint->cp1()->xChanged.clear();
    bezierPoint->cp1()->yChanged.clear();
    bezierPoint->cp2()->xChanged.clear();
    bezierPoint->cp2()->yChanged.clear();
}

void Curve::insertPoint(int index, ManagedObject<BezierPoint> bezierPoint, bool triggerCallback)
{
    BezierPointData data = bezierPoint->getData();

    {
        wg_write_lock_guard guard(pointsLock_);

        auto handle = bezierPoint.getHandleT();
		auto& points = pointsModel_.getSource();
        auto newPointItr = points.insertValue(index, handle);
        TF_ASSERT(newPointItr != points.end());

        auto objItr = std::begin(pointObjects_);
        std::advance(objItr, index);
        auto newObjItr = pointObjects_.insert(objItr, std::move(bezierPoint));
        TF_ASSERT(newObjItr != pointObjects_.end());

        addListeners(handle);
    }

    if (triggerCallback)
    {
        PointUpdateData added;
        added.point = data;
        added.index = index;
        added_(added);
    }
}

void Curve::removePoint(int index, bool triggerCallback)
{
    BezierPointData data = {};

    {
        wg_write_lock_guard guard(pointsLock_);

        auto& points = pointsModel_.getSource();
        auto pointsIter = std::begin(points);
        std::advance(pointsIter, index);
        TF_ASSERT(pointsIter != points.end());

        auto objItr = std::begin(pointObjects_);
        std::advance(objItr, index);
        TF_ASSERT(objItr != pointObjects_.end());

        auto handle = objItr->getHandleT();
        removeListeners(handle);
        data = handle->getData();

        points.erase(pointsIter);
        pointObjects_.erase(objItr);
    }

    if (triggerCallback)
    {
        PointUpdateData removed;
        removed.point = data;
        removed.index = index;
        removed_(removed);
    }
}

void Curve::insertPoint(BezierPointData data, bool updateYPos, bool triggerCallback)
{
    auto id = RefObjectId::generate().toString();
    TF_ASSERT(findIndex(id) == -1);
	ObjectHandleT<BezierPoint> prevPoint = nullptr;
	ObjectHandleT<BezierPoint> nextPoint = nullptr;
    {
        wg_read_lock_guard guard(pointsLock_);
        auto iter = std::begin(points_);
        for (; iter != std::end(points_); ++iter)
        {
            nextPoint = *iter;
            if (nextPoint->pos()->getX() > data.pos.x)
            {
                if (updateYPos)
                {
                    data.pos.y = nextPoint->pos()->getY();
                }
                break;
            }
            prevPoint = nextPoint;
        }

        if (iter == std::end(points_))
        {
            nextPoint = nullptr;
        }
    }

    auto nextId = nextPoint != nullptr ? nextPoint->id() : "";
    auto prevId = prevPoint != nullptr ? prevPoint->id() : "";
    auto nextData = nextPoint != nullptr ? nextPoint->getData() : BezierPointData();
    auto prevData = prevPoint != nullptr ? prevPoint->getData() : BezierPointData();

    // Object handles may not last the lifetime of the callbacks, do not store them in lambda
	auto executeFunc = [id, data, nextId, prevId, updateYPos, triggerCallback, this]()
    {
        auto nextPoint = findPoint(nextId);
        auto prevPoint = findPoint(prevId);
		auto newPointStorage = TypeClassDefinition<BezierPoint>::createObjectStorage(id, data);
		ManagedObject<BezierPoint> newPoint(newPointStorage, id);
		int index = 0;

        {
            wg_read_lock_guard guard(pointsLock_);

            for (auto iter = std::begin(points_); iter != std::end(points_); ++iter)
            {
                BezierPoint* point = (*iter).get();
                if (point->pos()->getX() > data.pos.x)
                    break;
                ++index;
            }

            // Adding the point to the end of the curve
            if (nextPoint == nullptr)
            {
                if (updateYPos)
                {
                    newPoint->pos()->setY(prevPoint != nullptr ? prevPoint->pos()->getY() : 0);
                }
                if (prevPoint != nullptr)
                {
                    interpolator_->updateControlPoints(*newPoint, prevPoint.get(), nullptr);
                }
                index = (int)points_.size();
            }
            else
            {
                // Are there both previous and next points?
                if (prevPoint != nullptr)
                {
                    auto t = interpolator_->timeAtX(data.pos.x, *prevPoint, *nextPoint);
                    if (updateYPos)
                    {
                        newPoint->setData(interpolator_->interpolate(t, *prevPoint, *nextPoint));
                    }
                    interpolator_->updateControlPoints(*newPoint, prevPoint.get(), nextPoint.get());
                }
                else
                {
                    // There is only a next point, update the control point
                    interpolator_->updateControlPoints(*newPoint, prevPoint.get(), nextPoint.get());
                }
            }
        }

        TF_ASSERT(findIndex(id) == -1);
        insertPoint(index, std::move(newPoint), triggerCallback);
        TF_ASSERT(findIndex(id) == index);
	};

    // Object handles may not last the lifetime of the callbacks, do not store them in lambda
	auto undoFunc = [id, data, nextId, prevId, nextData, prevData, triggerCallback, this]()
    {
        auto nextPoint = findPoint(nextId);
        auto prevPoint = findPoint(prevId);

        // Reset the next and previous points
		if (nextPoint != nullptr)
		{
            nextPoint->setData(nextData);
		}
		if (prevPoint != nullptr)
		{
            prevPoint->setData(prevData);
		}

        auto index = findIndex(id);
        if(index != -1)
        {
            removePoint(index, triggerCallback);
            TF_ASSERT(findIndex(id) == -1);
		}
	};

	pushModification(std::move(executeFunc), std::move(undoFunc));
}

void Curve::pushModification(ModificationFunction&& executeFunc, ModificationFunction&& undoFunc)
{
	while (currentState_ < modificationStack_.size() - 1)
	{
		modificationStack_.pop_back();
	}
	++currentState_;
	executeFunc();
	modificationStack_.emplace_back(std::move(executeFunc), std::move(undoFunc));
}
} // end namespace wgt
