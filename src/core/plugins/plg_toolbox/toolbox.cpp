//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  toolbox.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) 2017-2017 Wargaming World, Ltd. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "toolbox.hpp"
#include "toolbox_panel.hpp"
#include "toolbox/i_tool.hpp"

#include "core_common/assert.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_common/signal.hpp"
#include "core_reflection/property_accessor_listener.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_serialization/resizing_memory_stream.hpp"
#include "core_serialization_xml/xml_serializer.hpp"
#include "core_reflection/utilities/object_handle_reflection_utils.hpp"

namespace wgt
{
//==============================================================================
struct Toolbox::Impl : public Depends<IUIApplication, IDefinitionManager>
{
	typedef std::unordered_map<ITool*, std::unordered_map<IClassDefinition*, ObjectHandle>> PolymorphicPropertySettings;

	//------------------------------------------------------------------------------
	struct PropertyListener : public PropertyAccessorListener
	{
		PropertyListener(Toolbox::Impl& impl, const std::vector<ObjectHandleT<ITool>>& tools,
		                 PolymorphicPropertySettings& toolSettings);
		void preSetValue(const PropertyAccessor& accessor, const Variant& value) override;
		void postSetValue(const PropertyAccessor& accessor, const Variant& value) override;
		ObjectHandleT<ITool> getTool(const PropertyAccessor& accessor);
		Toolbox::Impl& impl_;
		const std::vector<ObjectHandleT<ITool>>& tools_;
		PolymorphicPropertySettings& toolSettings_;
	};

	Impl(ToolboxPanel* toolboxPanel);
	~Impl();

	std::shared_ptr<PropertyAccessorListener> propListener_;
	ToolboxPanel* toolboxPanel_;
	std::vector<ObjectHandleT<ITool>> tools_;
	ObjectHandleT<ITool> activeTool_;
	std::string defaultIcon_;
	PolymorphicPropertySettings toolPolymorphicPropertySettings_;
	Signal<VoidSignature> toolSetChanged_;
	Signal<ActiveToolSignature> activeToolChanged_;
	ConnectionHolder connections_;
	ObjectHandleT<ITool> defaultTool_;
};

//------------------------------------------------------------------------------
Toolbox::Impl::PropertyListener::PropertyListener(Toolbox::Impl& impl, const std::vector<ObjectHandleT<ITool>>& tools,
                                                  PolymorphicPropertySettings& toolSettings)
    : impl_(impl), tools_(tools), toolSettings_(toolSettings)
{
}

//------------------------------------------------------------------------------
ObjectHandleT<ITool> Toolbox::Impl::PropertyListener::getTool(const PropertyAccessor& accessor)
{
	auto defManager = accessor.getDefinitionManager();
	auto rootObject = accessor.getRootObject();
	return reflectedCast<ITool>(rootObject, *defManager);
}

//------------------------------------------------------------------------------
void Toolbox::Impl::PropertyListener::preSetValue(const PropertyAccessor& accessor, const Variant&)
{
	auto tool = getTool(accessor);
	if (tool == nullptr)
	{
		return;
	}
	auto it = std::find(tools_.begin(), tools_.end(), tool);
	TF_ASSERT(it != tools_.end());
	auto previousValue = accessor.getValue();
	ObjectHandle handle;
	if (!previousValue.tryCast(handle))
	{
		return;
	}
	auto defManager = accessor.getDefinitionManager();
	handle = reflectedRoot(handle, *defManager);
	if (handle == nullptr)
	{
		return;
	}
	auto objectDefinition = defManager->getObjectDefinition(handle);
	if (objectDefinition == nullptr)
	{
		return;
	}
	auto findIt = toolSettings_.find(tool.get());
	TF_ASSERT(findIt != toolSettings_.end());
	auto& settings = findIt->second;
	if (auto found = settings.find(objectDefinition) == settings.end())
	{
		settings[objectDefinition] = handle;
		return;
	}
	auto savedHandle = settings[objectDefinition];
	auto saveObjectDefinition = defManager->getObjectDefinition(savedHandle);
	TF_ASSERT(saveObjectDefinition == objectDefinition);
	if (savedHandle == handle)
	{
		return;
	}
	// store new object handle
	settings[objectDefinition] = handle;
}

//------------------------------------------------------------------------------
void Toolbox::Impl::PropertyListener::postSetValue(const PropertyAccessor& accessor, const Variant& value)
{
	auto tool = getTool(accessor);
	if (tool == nullptr)
	{
		return;
	}
	auto it = std::find(tools_.begin(), tools_.end(), tool);
	TF_ASSERT(it != tools_.end());

	ObjectHandle handle;
	if (!value.tryCast(handle))
	{
		return;
	}
	auto defManager = accessor.getDefinitionManager();
	handle = reflectedRoot(handle, *defManager);
	if (handle == nullptr)
	{
		return;
	}
	auto objectDefinition = defManager->getObjectDefinition(handle);
	if (objectDefinition == nullptr)
	{
		return;
	}
	auto findIt = toolSettings_.find(tool.get());
	TF_ASSERT(findIt != toolSettings_.end());
	auto& settings = findIt->second;
	if (auto found = settings.find(objectDefinition) == settings.end())
	{
		return;
	}
	auto savedHandle = settings[objectDefinition];
	auto saveObjectDefinition = defManager->getObjectDefinition(savedHandle);
	TF_ASSERT(saveObjectDefinition == objectDefinition);
	if (savedHandle == handle)
	{
		return;
	}
	// restore data from the saved handle
	const PropertyIteratorRange& props = objectDefinition->allProperties();
	for (PropertyIterator pi = props.begin(); pi != props.end(); ++pi)
	{
		std::string parentPath = pi->getName();
		PropertyAccessor propToSet = objectDefinition->bindProperty(parentPath.c_str(), handle);
		PropertyAccessor propToGet = objectDefinition->bindProperty(parentPath.c_str(), savedHandle);
		TF_ASSERT(propToSet.isValid() && propToGet.isValid());
		if (propToSet.getProperty()->isMethod())
		{
			continue;
		}
		auto savedValue = propToGet.getValue();
		propToSet.setValueWithoutNotification(savedValue);
	}
}

//------------------------------------------------------------------------------
Toolbox::Impl::Impl(ToolboxPanel* toolboxPanel)
    : propListener_(new PropertyListener(*this, tools_, toolPolymorphicPropertySettings_))
	, toolboxPanel_(toolboxPanel)
	, activeTool_(nullptr)
	, defaultTool_(nullptr)
{
	auto defManager = get<IDefinitionManager>();
	TF_ASSERT(defManager != nullptr);
	defManager->registerPropertyAccessorListener(propListener_);
}

//------------------------------------------------------------------------------
Toolbox::Impl::~Impl()
{
	auto defManager = get<IDefinitionManager>();
	TF_ASSERT(defManager != nullptr);
	defManager->deregisterPropertyAccessorListener(propListener_);
	propListener_ = nullptr;
}

Toolbox::Toolbox(ToolboxPanel* toolboxPanel) : impl_(std::make_unique<Toolbox::Impl>(toolboxPanel))
{
}

Toolbox::~Toolbox()
{
}

void Toolbox::registerTool(ObjectHandleT<ITool> tool)
{
	if (tool != nullptr && std::find(impl_->tools_.begin(), impl_->tools_.end(), tool) == impl_->tools_.end())
	{
		impl_->tools_.push_back(tool);
		impl_->toolPolymorphicPropertySettings_[tool.get()] = std::unordered_map<IClassDefinition*, ObjectHandle>();
		impl_->toolSetChanged_();
	}
}

void Toolbox::deregisterTool(ObjectHandleT<ITool> tool)
{
	auto findIt = impl_->toolPolymorphicPropertySettings_.find(tool.get());
	if (findIt != impl_->toolPolymorphicPropertySettings_.end())
	{
		impl_->toolPolymorphicPropertySettings_.erase(findIt);
	}

	auto it = std::find(impl_->tools_.begin(), impl_->tools_.end(), tool);
	if (it != impl_->tools_.end())
	{
		if(tool == impl_->defaultTool_)
		{
			this->setDefaultTool(nullptr);
		}
		impl_->tools_.erase(it);
		impl_->toolSetChanged_();
	}
}

bool Toolbox::setDefaultTool(ObjectHandleT<ITool> tool)
{
	if(tool != nullptr)
	{
		if(std::find(impl_->tools_.begin(), impl_->tools_.end(), tool) == impl_->tools_.end())
		{
			return false;
		}
	}

	impl_->defaultTool_ = tool;

	if(impl_->activeTool_ == nullptr)
	{
		return setActiveTool(tool);
	}

	return true;
}

ObjectHandleT<ITool> Toolbox::getDefaultTool()
{
	return impl_->defaultTool_;
}

bool Toolbox::setActiveTool(ObjectHandleT<ITool> tool)
{
	if (tool != nullptr &&
	    (std::find(impl_->tools_.begin(), impl_->tools_.end(), tool) == impl_->tools_.end() || !tool->canActivate()))
	{
		return false;
	}

	if (impl_->activeTool_ == tool)
	{
		return false;
	}

	if (impl_->activeTool_ != nullptr)
	{
		impl_->activeTool_->deactivate();
	}

	ObjectHandleT<ITool> newTool;

	if(tool == nullptr)
	{
		if(impl_->defaultTool_.get() && impl_->defaultTool_->canActivate())
		{
			newTool = impl_->defaultTool_;
		}
		else
		{
			newTool = nullptr;
		}
	}
	else
	{
		newTool = tool;
	}

	impl_->activeTool_ = newTool;

	if (impl_->activeTool_ != nullptr)
	{
		impl_->activeTool_->activate();
	}

	impl_->activeToolChanged_(newTool);
	return true;
}

ObjectHandleT<ITool> Toolbox::getActiveTool() const
{
	return impl_->activeTool_;
}

ObjectHandleT<ITool> Toolbox::getTool(uint64_t id) const
{
	for (auto& tool : impl_->tools_)
	{
		if(tool.get() != nullptr && tool->id() == id)
			return tool;
	}
	return nullptr;
}

std::vector<ObjectHandleT<ITool>> Toolbox::getAllTools() const
{
	return impl_->tools_;
}

const char* Toolbox::getDefaultIcon() const
{
	return impl_->defaultIcon_.c_str();
}

void Toolbox::setDefaultIcon(const char* icon)
{
	impl_->defaultIcon_ = icon;
}

void Toolbox::activatePanel()
{
	if (impl_->toolboxPanel_ != nullptr)
	{
		impl_->toolboxPanel_->activate();
	}
}

void Toolbox::init()
{
	if (auto uiApplication = impl_->get<IUIApplication>())
	{
		impl_->connections_ += uiApplication->signalUpdate.connect([this] {
			if (impl_->activeTool_ != nullptr)
			{
				impl_->activeTool_->update();
			}
		});
	}
}

void Toolbox::fini()
{
}

Connection Toolbox::connectToolsetChanged(ToolsetChangedCallback callback)
{
	return impl_->toolSetChanged_.connect(callback);
}

Connection Toolbox::connectActiveToolChanged(ActiveToolChangedCallback callback)
{
	return impl_->activeToolChanged_.connect(callback);
}

} // end namespace wgt
