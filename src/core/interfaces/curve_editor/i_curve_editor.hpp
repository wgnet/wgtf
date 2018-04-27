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
#include <core_data_model/abstract_item_model.hpp>
#include <core_reflection/reflected_object.hpp>
#include <core_dependency_system/i_interface.hpp>
#include <core_dependency_system/depends.hpp>
#include <core_reflection/i_definition_manager.hpp>
#include <core_reflection/property_accessor.hpp>
#include <core_object/object_handle_provider.hpp>

namespace wgt
{
typedef ObjectHandleT<class ICurve> ICurveHandle;
typedef ManagedObjectPtr ICurvePtr;

struct ScaleChangeData
{
	float xScale;
	float yScale;
};

class ICurveEditor : Depends<IDefinitionManager>
				   , public ObjectHandleProvider<ICurveEditor>
{
	DECLARE_REFLECTED;

public:
    virtual ~ICurveEditor() = default;

    /**
    * Adds a new curve owned by the editor
    * @return handle to the new curve or null if not successful
    */
	virtual ICurveHandle addCurve(ICurvePtr /*curve*/) = 0;

    /**
    * Adds a new curve owned by the editor
    * @return handle to the new curve or null if not successful
    */
    virtual ICurveHandle addCurve(CurveTypes::CurveType /*curveType*/) = 0;

    /**
    * Adds a new curve not owned by the editor
    * @return whether adding was successful
    */
    virtual bool addCurve(ICurveHandle /*curve*/) = 0;

	virtual ICurvePtr createCurve(CurveTypes::CurveType /*curveType*/) = 0;

	virtual bool getAllowEmptyCurves() const = 0;

    virtual void clear() = 0;

	virtual const std::string& getSubTitle() const = 0;

	virtual const float& getXScale() const = 0;

	virtual const float& getYScale() const = 0;

	virtual const bool& getTimeScaleEditEnabled() const = 0;

	virtual bool removeCurve(ICurveHandle /*curve*/) = 0;

	void setAllowEmptyCurves(bool allowEmptyCurves)
	{
		setReflectedProperty("allowEmptyCurves", allowEmptyCurves);
	}

	void setSubTitle(const char* subTitle)
	{
        setReflectedProperty("subTitle", subTitle);
	}

	void setXScale(const float& xScale)
	{
        setReflectedProperty("xScale", xScale);
	}

	void setYScale(const float& yScale)
	{
        setReflectedProperty("yScale", yScale);
	}

	void setTimeScaleEditEnabled(const bool& timeScaleEditEnabled)
	{
        setReflectedProperty("timeScaleEditEnabled", timeScaleEditEnabled);
	}

	typedef std::function<void(ScaleChangeData)> ScaleChangeCallback;
	virtual void connectOnScaleChange(ScaleChangeCallback) = 0;

	virtual void setUIVisible(bool visible) = 0;

private:

    // TODO: Fix this, we shouldn't be depending on exposed names in the header
    template<typename T>
    void setReflectedProperty(const char* name, T& value)
    {
        auto defManager = get<IDefinitionManager>();
        auto definition = defManager->getDefinition<ICurveEditor>();
        PropertyAccessor pa = definition->bindProperty(name, handle());
        pa.setValue(value);
    }

	virtual const AbstractListModel* getCurves() const = 0;

	virtual void putAllowEmptyCurves(const bool& /*allowEmptyCurves*/) = 0;

	virtual void putSubTitle(const std::string& /*subTitle*/) = 0;

	virtual void putXScale(const float&) = 0;

	virtual void putYScale(const float&) = 0;

	virtual void putTimeScaleEditEnabled(const bool&) = 0;
};
} // end namespace wgt
#endif // i_curve_editor_H_
