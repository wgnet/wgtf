//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  i_curve_editor.hpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef i_curve_editor_H_
#define i_curve_editor_H_

#pragma once

#include <assert.h>
#include "curve_types.hpp"

#include <core_reflection/object_handle.hpp>
#include <core_data_model/i_list_model.hpp>
#include <core_reflection/reflected_object.hpp>

#include <core_dependency_system/i_interface.hpp>
#include <core_reflection/i_definition_manager.hpp>
#include <core_reflection/property_accessor.hpp>

namespace wgt
{
class ICurve;
typedef std::shared_ptr<ICurve> ICurvePtr;

#define SET_REFLECTED_PROPERTY(prop) \
	auto definition = Context::queryInterface<IDefinitionManager>()->getDefinition<ICurveEditor>(); \
	auto handle = ObjectHandleT<ICurveEditor>(this, definition); \
	PropertyAccessor pa = definition->bindProperty(#prop, handle); \
	pa.setValue(prop);

struct ScaleChangeData
{
	float xScale;
	float yScale;
};

class ICurveEditor
{
	DECLARE_REFLECTED;
public:
	virtual ~ICurveEditor(){}

	virtual bool addCurve(ICurvePtr /*curve*/) = 0;

	virtual void clear() = 0;

	virtual ICurvePtr createCurve(CurveTypes::CurveType /*curveType*/, bool /*add*/) = 0;

	virtual const std::string& getSubTitle() const = 0;

	virtual const float& getXScale() const = 0;
	
	virtual const float& getYScale() const = 0;

	virtual const bool& getTimeScaleEditEnabled() const = 0;

	virtual bool removeCurve(ICurvePtr /*curve*/) = 0;

	void setSubTitle(const char* subTitle)
	{
		SET_REFLECTED_PROPERTY(subTitle);
	}
	
	void setXScale(const float& xScale)
	{
		SET_REFLECTED_PROPERTY(xScale);
	}

	void setYScale(const float& yScale)
	{
		SET_REFLECTED_PROPERTY(yScale);
	}

	void setTimeScaleEditEnabled(const bool& timeScaleEditEnabled)
	{
		SET_REFLECTED_PROPERTY(timeScaleEditEnabled);
	}

	typedef std::function<void(ScaleChangeData)> ScaleChangeCallback;
	virtual void connectOnScaleChange(ScaleChangeCallback) = 0;
	
private:
	virtual const IListModel* getCurves() const = 0;

	virtual void putSubTitle(const std::string& /*subTitle*/) = 0;

	virtual void putXScale(const float&) = 0;

	virtual void putYScale(const float&) = 0;

	virtual void putTimeScaleEditEnabled(const bool&) = 0;

};
} // end namespace wgt
#endif // i_curve_editor_H_
