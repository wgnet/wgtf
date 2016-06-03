//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  curve_editor.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "models/curve_editor.hpp"
#include "models/curve.hpp"

#include <core_data_model/i_item_role.hpp>

#include "interpolators/interpolator_factory.hpp"

namespace wgt
{
ICurvePtr CurveEditor::createCurve(CurveTypes::CurveType curveType, bool add)
{
	auto definitionManager = Context::queryInterface<IDefinitionManager>();
	if(definitionManager)
	{
		// This method of creating a curve requires a default constructor
		//auto curveDef = definitionManager->getDefinition<ICurve>();
		//if(curveDef)
		//{
		//	auto curve = TypeClassDefinition<Curve>::create(*curveDef, definitionManager);
		//	curves_.emplace_back(curve);
		//	return curve.get();
		//}

		// Create a curve and pass it by reference to our exposed model for variants
		ICurvePtr pCurve = std::make_shared<Curve>(createInterpolator(curveType), definitionManager);

		if( add )
		{
			storage_.emplace_back(pCurve);
			curves_.emplace_back(storage_.back().get());
		}

		return pCurve;
	}
	return nullptr;
}

bool CurveEditor::addCurve(ICurvePtr curve)
{
	for( auto iter = storage_.begin(); iter != storage_.end(); ++iter )
	{
		ICurvePtr current = *iter;
		if( current.get() == curve.get() )
			return false;
	}

	storage_.emplace_back(curve);
	curves_.emplace_back(storage_.back().get());

	return true;
}

bool CurveEditor::removeCurve(ICurvePtr curve)
{
	auto storeIter = storage_.begin();
	for(auto iter = curves_.begin(); iter != curves_.end(); ++iter, ++ storeIter)
	{
		auto current = &*storeIter->get();
		if(current == curve.get())
		{
			curves_.erase(iter);
			storage_.erase(storeIter);
			return true;
		}
	}
	return false;
}

void CurveEditor::clear()
{
	curves_.clear();
	storage_.clear();
	scaleChangeSignal_.clear();
	setSubTitle("");
	setXScale(1.0f);
	setYScale(1.0f);
	timeScaleEditEnabled_ = true;
}

void CurveEditor::putSubTitle(const std::string& subTitle)
{
	subTitle_ = subTitle;
}

void CurveEditor::putXScale(const float& xScale)
{ 
	xScale_ = xScale;

	ScaleChangeData newScale;
	newScale.xScale = xScale_;
	newScale.yScale = yScale_;
	scaleChangeSignal_(newScale);
}

void CurveEditor::putYScale(const float& yScale)
{ 
	yScale_ = yScale; 

	ScaleChangeData newScale;
	newScale.xScale = xScale_;
	newScale.yScale = yScale_;
	scaleChangeSignal_(newScale);
}

void CurveEditor::putTimeScaleEditEnabled(const bool& timeScaleEditEnabled)
{
	timeScaleEditEnabled_ = timeScaleEditEnabled;
}
} // end namespace wgt
