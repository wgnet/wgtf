//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  curve_editor.hpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef CURVES_MODEL_H_
#define CURVES_MODEL_H_

#pragma once

#include "interfaces/i_curve_editor.hpp"
#include "interfaces/curve_types.hpp"

#include "core_reflection/reflected_object.hpp"
#include "core_reflection/type_class_definition.hpp"

#include <core_common/signal.hpp>

// TODO: remove this pragma
#pragma warning(push)
#pragma warning(disable : 4996)
#include "core_data_model/generic_list.hpp"
#pragma warning(pop)

namespace wgt
{
class CurveEditor : public Implements<ICurveEditor>
{
	DECLARE_REFLECTED

	typedef Signal<void(ScaleChangeData)> ScaleChangeSignal;

public:
	CurveEditor() : xScale_(1.0f), yScale_(1.0f), timeScaleEditEnabled_(true)
	{
	}
	virtual ~CurveEditor(){};

	virtual ICurvePtr createCurve(CurveTypes::CurveType curveType = CurveTypes::Linear, bool add = false) override;
	virtual bool addCurve(ICurvePtr curve) override;
	virtual bool removeCurve(ICurvePtr curve) override;
	virtual void clear() override;
	virtual const std::string& getSubTitle() const override
	{
		return subTitle_;
	}
	virtual const float& getXScale() const override
	{
		return xScale_;
	}
	virtual const float& getYScale() const override
	{
		return yScale_;
	}
	virtual const bool& getTimeScaleEditEnabled() const override
	{
		return timeScaleEditEnabled_;
	}

	virtual void connectOnScaleChange(ScaleChangeCallback cb) override
	{
		scaleChangeSignal_.connect(std::move(cb));
	}

protected:
	virtual void putSubTitle(const std::string& subTitle) override;
	virtual void putXScale(const float& xScale) override;
	virtual void putYScale(const float& yScale) override;
	virtual void putTimeScaleEditEnabled(const bool& timeScaleEditEnabled) override;

	const IListModel* getCurves() const override
	{
		return &curves_;
	}

private:
	std::string subTitle_;
	float xScale_;
	float yScale_;
	bool timeScaleEditEnabled_;
	GenericListT<ObjectHandleT<ICurve>> curves_;
	std::vector<std::shared_ptr<ICurve>> storage_;

	ScaleChangeSignal scaleChangeSignal_;
};
} // end namespace wgt
#endif // CURVES_MODEL_H_
