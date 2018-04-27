//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  curve_editor.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "models/curve_editor.hpp"

#include "models/curve.hpp"
#include "core_common/assert.hpp"
#include "core_data_model/i_item_role.hpp"
#include "interpolators/interpolator_factory.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_common/wg_future.hpp"
#include "core_ui_framework/i_view.hpp"

namespace wgt
{
struct CurveEditor::Impl
{
	Impl()
	 : xScale_(1.0f)
	 , yScale_(1.0f)
	 , timeScaleEditEnabled_(true)
	 , allowEmptyCurves_(false)
	 , bPanelVisible_(false)
	{

	}
	~Impl() {}
	std::string subTitle_;
	float xScale_;
	float yScale_;
	bool timeScaleEditEnabled_;
	bool allowEmptyCurves_;
	CollectionModel curves_;
	std::vector<ICurveHandle> storage_;
	std::vector<ICurvePtr> ownedCurves_;
	ScaleChangeSignal scaleChangeSignal_;
	bool bPanelVisible_;
	wg_shared_future<std::unique_ptr<IView>> curvePanel_;
};

CurveEditor::CurveEditor()
	: impl_(new Impl)
{
	impl_->curves_.setSource(Collection(impl_->storage_));
}

CurveEditor::~CurveEditor()
{
	impl_ = nullptr;
}

void CurveEditor::init(const ObjectHandle& context)
{
	DependsLocal<IViewCreator, IUIApplication> depends;
	auto viewCreator = depends.get<IViewCreator>();
	auto uiApplication = depends.get<IUIApplication>();
	if (viewCreator != nullptr && uiApplication != nullptr)
	{
		impl_->bPanelVisible_ = true;
		impl_->curvePanel_ = viewCreator->createView("plg_curve_editor/CurveEditor.qml", context, [this, uiApplication](IView& view) {
			if (uiApplication)
			{
				if (!impl_->bPanelVisible_)
				{
					uiApplication->removeView(view);
				}
			}
		});
	}
}

void CurveEditor::fini()
{
	clear();
	setUIVisible(false);
	impl_->curvePanel_ = wg_shared_future<std::unique_ptr<IView>>();
}

ICurvePtr CurveEditor::createCurve(CurveTypes::CurveType curveType)
{
	return ManagedObject<Curve>::make_unique(createInterpolator(curveType));
}

ICurveHandle CurveEditor::addCurve(CurveTypes::CurveType curveType)
{
    return addCurve(createCurve(curveType));
}

bool CurveEditor::addCurve(ICurveHandle curve)
{
    for (auto itr = impl_->storage_.begin(); itr != impl_->storage_.end(); ++itr)
    {
        if (*itr == curve)
        {
            return false;
        }
    }

    Collection& collection = impl_->curves_.getSource();
    collection.insertValue(collection.size(), curve);
    TF_ASSERT(impl_->storage_.size() == collection.size());

    return true;
}

ICurveHandle CurveEditor::addCurve(ICurvePtr curve)
{
    auto handle = curve->getHandleT<ICurve>();
    TF_ASSERT(handle != nullptr);
    if (addCurve(handle))
    {
		impl_->ownedCurves_.push_back(std::move(curve));
        return handle;
    }
    return nullptr;
}

bool CurveEditor::removeCurve(ICurveHandle curve)
{
    int index = 0;
    for (auto itr = impl_->storage_.begin(); itr != impl_->storage_.end(); ++itr, ++index)
    {
        if (*itr == curve)
        {
            Collection& collection = impl_->curves_.getSource();
            auto handleItr = collection.begin();
            std::advance(handleItr, index);
            collection.erase(handleItr);

            // Also remove if the curve is owned by the editor
            auto ownedItr = std::remove_if(impl_->ownedCurves_.begin(), impl_->ownedCurves_.end(),
                [&curve](const ICurvePtr& obj) { return obj->getHandle() == curve; });
            if (ownedItr != impl_->ownedCurves_.end())
            {
				impl_->ownedCurves_.erase(ownedItr, impl_->ownedCurves_.end());
            }
           
            return true;
        }
    }
	return false;
}

void CurveEditor::clear()
{
	Collection& collection = impl_->curves_.getSource();
	collection.erase(collection.begin(), collection.end());
	impl_->scaleChangeSignal_.clear();
	impl_->storage_.clear();
	impl_->ownedCurves_.clear();
	setSubTitle("");
	setXScale(1.0f);
	setYScale(1.0f);
	impl_->timeScaleEditEnabled_ = true;
}

bool CurveEditor::getAllowEmptyCurves() const
{
	return impl_->allowEmptyCurves_;
}

const std::string& CurveEditor::getSubTitle() const
{
	return impl_->subTitle_;
}

const float& CurveEditor::getXScale() const
{
	return impl_->xScale_;
}

const float& CurveEditor::getYScale() const
{
	return impl_->yScale_;
}

const bool& CurveEditor::getTimeScaleEditEnabled() const
{
	return impl_->timeScaleEditEnabled_;
}

void CurveEditor::connectOnScaleChange(ScaleChangeCallback cb)
{
	impl_->scaleChangeSignal_.connect(std::move(cb));
}

void CurveEditor::putAllowEmptyCurves(const bool& allowEmptyCurves)
{
	impl_->allowEmptyCurves_ = allowEmptyCurves;
}

void CurveEditor::putSubTitle(const std::string& subTitle)
{
	impl_->subTitle_ = subTitle;
}

void CurveEditor::putXScale(const float& xScale)
{
	impl_->xScale_ = xScale;

	ScaleChangeData newScale;
	newScale.xScale = impl_->xScale_;
	newScale.yScale = impl_->yScale_;
	impl_->scaleChangeSignal_(newScale);
}

void CurveEditor::putYScale(const float& yScale)
{
	impl_->yScale_ = yScale;

	ScaleChangeData newScale;
	newScale.xScale = impl_->xScale_;
	newScale.yScale = impl_->yScale_;
	impl_->scaleChangeSignal_(newScale);
}

void CurveEditor::putTimeScaleEditEnabled(const bool& timeScaleEditEnabled)
{
	impl_->timeScaleEditEnabled_ = timeScaleEditEnabled;
}

const AbstractListModel* CurveEditor::getCurves() const
{
	return &impl_->curves_;
}

void CurveEditor::setUIVisible(bool visible)
{
	if (impl_->bPanelVisible_ == visible)
	{
		return;
	}

	impl_->bPanelVisible_ = visible;
	if (!impl_->curvePanel_.valid())
	{
		return;
	}

	DependsLocal<IUIApplication> depends;
	auto uiApplication = depends.get<IUIApplication>();
	if (uiApplication == nullptr)
	{
		return;
	}

	IView& view = *impl_->curvePanel_.get();
	if (visible)
	{
		//show panel
		uiApplication->addView(view);
	}
	else
	{
		//hide panel
		uiApplication->removeView(view);
	}
}

} // end namespace wgt
