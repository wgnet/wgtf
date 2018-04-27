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

#include "curve_editor/i_curve_editor.hpp"
#include "curve_editor/curve_types.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/type_class_definition.hpp"
#include "core_data_model/collection_model.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_common/signal.hpp"

namespace wgt
{
class CurveEditor : public Implements<ICurveEditor>
{
	DECLARE_REFLECTED

	typedef Signal<void(ScaleChangeData)> ScaleChangeSignal;

public:
	CurveEditor();
	virtual ~CurveEditor();

	void init(const ObjectHandle& context);
	void fini();

	virtual ICurvePtr createCurve(CurveTypes::CurveType curveType = CurveTypes::Linear) override;
    virtual ICurveHandle addCurve(CurveTypes::CurveType curveType = CurveTypes::Linear) override;
	virtual ICurveHandle addCurve(ICurvePtr curve) override;
    virtual bool addCurve(ICurveHandle curve) override;
	virtual bool removeCurve(ICurveHandle curve) override;
	virtual void clear() override;
	virtual bool getAllowEmptyCurves() const override;
	virtual const std::string& getSubTitle() const override;
	virtual const float& getXScale() const override;
	virtual const float& getYScale() const override;
	virtual const bool& getTimeScaleEditEnabled() const override;
	virtual void connectOnScaleChange(ScaleChangeCallback cb) override;
	virtual void setUIVisible(bool visible) override;

protected:
	virtual void putAllowEmptyCurves(const bool& allowEmptyCurves) override;
	virtual void putSubTitle(const std::string& subTitle) override;
	virtual void putXScale(const float& xScale) override;
	virtual void putYScale(const float& yScale) override;
	virtual void putTimeScaleEditEnabled(const bool& timeScaleEditEnabled) override;
	virtual const AbstractListModel* getCurves() const override;


private:
	struct Impl;
	std::unique_ptr<struct Impl> impl_;
};
} // end namespace wgt
#endif // CURVES_MODEL_H_
