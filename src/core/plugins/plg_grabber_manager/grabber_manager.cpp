#include "grabber_manager.hpp"
#include "preference_utils.hpp"

#include "core_common/assert.hpp"
#include "interfaces/grabber_manager/i_grabber.hpp"
#include "core_reflection/class_definition.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_variant/variant.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_common/wg_future.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_dependency_system/context_callback_helper.hpp"
#include "core_object/managed_object.hpp"
#include "core_data_model/reflection_proto/property_tree_model.hpp"
#include "core_view_model/property_view_model.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_reflection/reflected_method_parameters.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_string_utils/string_utils.hpp"
#include "core_ui_framework/i_preferences.hpp"
#include "core_logging/logging.hpp"

#include <unordered_map>
#include <vector>
#include <mutex>
#include <sstream>

namespace wgt
{
	static const char* GRABBER_MANAGER_COMPONENT_ID = "tfgrabbermanager";

	struct GrabberDetails
	{
		std::vector<IBasePropertyPtr> uniqueProperties_;
		std::map<EnvironmentId, std::vector<Variant>> uniqueValues_;
		std::vector<Variant> defaultValues_;
		std::vector<std::unique_ptr<IAction>> actions_;
		ObjectHandle handle_; // For checking if the grabber's wrapped object has changed.
		bool initialized_ = false;
		bool visible_ = true;
	};

	struct GrabberManager::Implementation
		: public Depends<IEnvManager, IDefinitionManager, IApplication, IUIApplication, IViewCreator, IUIFramework, IPreferences>
		, ContextCallBackHelper
		, EnvComponentT<EmptyEnvComponentState>
	{
		Implementation(GrabberManager& self);
		virtual ~Implementation();

		const char* getEnvComponentId() const override;
		void onEnvironmentCreated(const EnvironmentId& envId) override;
		void onEnvironmentRemoved(const EnvironmentId& envId) override;
		void onPreEnvironmentChanged(const EnvironmentId& oldId, const EnvironmentId& newId) override;
		void onPostEnvironmentChanged(const EnvironmentId& oldId, const EnvironmentId& newId) override;

		void updatePanel();
		void createPanel();
		void destroyPanel();
		void addActions(IGrabber* grabber, std::function<bool()> iconVisibleFn);

		void addAction(IGrabber* grabber, std::function<bool()> iconVisibleFn,
			ObjectHandle object, IBasePropertyPtr property);

		void removeActions(IGrabber* grabber);

		bool registerGrabber(IGrabber* grabber, std::function<bool()> iconVisibleFn = nullptr);
		bool deregisterGrabber(IGrabber* grabber);
		void setGrabberVisibility(IGrabber& grabber, GrabberDetails& details, bool visible);

		void postLoad();
		void init();
		void fini();

		void loadEnvironmentSettings(const EnvironmentId& envId);
		void saveEnvironmentSettings(const EnvironmentId& envId);
		void loadGlobalSettings(IGrabber* grabber, GrabberDetails& details);
		void saveGlobalSettings(IGrabber* grabber);
		void loadSettingsDefinitions(IPreferences& preferences, IDefinitionManager& manager);
		void saveSettingsDefinitions(IPreferences& preferences, IDefinitionManager& manager);
		void storeEnvironmentSettings(GrabberDetails& grabber, const EnvironmentId& id);
		void restoreEnvironmentSettings(GrabberDetails& grabber, const EnvironmentId& id);

		GrabberManager& self_;
		wg_future<std::unique_ptr<IView>> settingsPanel_;
		std::unordered_map<IGrabber*, GrabberDetails> grabbers_;
		mutable std::mutex grabbersMutex_;
		ManagedObject<PropertyViewModel> settingsViewModel_;
		ManagedObject<GenericObject> settingsContainer_;
		Connection updateConnection_;
		bool refresh_ = false;
		bool loadedSettingsDefinitions_ = false;
	};

	GrabberManager::Implementation::Implementation(GrabberManager& self)
		: self_(self), EnvComponentT(*get<IEnvManager>())
	{
		initEnvComponent();

		registerCallback([this](IApplication& application)
		{
			updateConnection_ = application.signalUpdate.connect(
				std::bind(&GrabberManager::update, &self_));
		});

		registerCallback(
			[this](IPreferences& preferences, IDefinitionManager& manager) 
		{
			loadSettingsDefinitions(preferences, manager); 
		},
			[this](IPreferences& preferences, IDefinitionManager& manager) 
		{
			saveSettingsDefinitions(preferences, manager); 
		});
	}

	GrabberManager::Implementation::~Implementation()
	{
		updateConnection_.disconnect();
		finiEnvComponent();
	}

	const char* GrabberManager::Implementation::getEnvComponentId() const
	{
		return GRABBER_MANAGER_COMPONENT_ID;
	}

	void GrabberManager::Implementation::storeEnvironmentSettings(GrabberDetails& grabber, const EnvironmentId& id)
	{
		if(auto definitionManager = get<IDefinitionManager>())
		{
			auto& props = grabber.uniqueProperties_;
			auto& values = grabber.uniqueValues_[id];
			TF_ASSERT(props.size() == values.size());
			for (size_t i = 0; i < props.size(); ++i)
			{
				values[i] = props[i]->get(grabber.handle_, *definitionManager);
			}
		}
	}

	void GrabberManager::Implementation::restoreEnvironmentSettings(GrabberDetails& grabber, const EnvironmentId& id)
	{
		if(auto definitionManager = get<IDefinitionManager>())
		{
			auto& props = grabber.uniqueProperties_;
			auto& values = grabber.uniqueValues_[id];
			TF_ASSERT(props.size() == values.size());
			for (size_t i = 0; i < props.size(); ++i)
			{
				props[i]->set(grabber.handle_, values[i], *definitionManager);
			}
		}
	}

	void GrabberManager::Implementation::loadSettingsDefinitions(IPreferences& preferences, IDefinitionManager& manager)
	{
		if (preferences.preferenceExists(GRABBER_MANAGER_COMPONENT_ID))
		{
			auto& prefFile = preferences.getPreference(GRABBER_MANAGER_COMPONENT_ID);
			loadedSettingsDefinitions_ = PreferenceUtils::deserializeDefinitions(manager, prefFile);
		}
	}

	void GrabberManager::Implementation::saveSettingsDefinitions(IPreferences& preferences, IDefinitionManager& manager)
	{
		auto& prefFile = preferences.getPreference(GRABBER_MANAGER_COMPONENT_ID);
		PreferenceUtils::serializeDefinitions(manager, prefFile);
	}

	void GrabberManager::Implementation::loadGlobalSettings(IGrabber* grabber, GrabberDetails& details)
	{
		auto preferences = get<IPreferences>();
		auto definitionManager = get<IDefinitionManager>();
		if (!preferences || !definitionManager)
		{
			return;
		}

		if (!preferences->preferenceExists(GRABBER_MANAGER_COMPONENT_ID))
		{
			return;
		}

		const char* id = grabber->getId();
		auto& prefFile = preferences->getPreference(GRABBER_MANAGER_COMPONENT_ID);
		if (!prefFile->findProperty(id).isValid())
		{
			return;
		}

		auto object = grabber->getPropertiesObject();
		auto definition = definitionManager->getDefinition(object);
		if (!definition)
		{
			return;
		}

		const auto& unique = grabber->getUniquePropertyNames();
		auto isGlobalProperty = [&unique](const char* path) -> bool
		{
			return unique.empty() || 
				std::find(unique.begin(), unique.end(), path) == unique.end();
		};

		std::string errors;
		auto prefObj = GenericObject::create();
		if (PreferenceUtils::deserialize(*definitionManager, id, prefFile, prefObj, errors))
		{
			auto prefHandle = prefObj.getHandleT();
			auto properties = definition->allProperties();
			for (const auto& prop : properties)
			{
				PreferenceUtils::loadPropertyFromGeneric(
					*definitionManager, *definition, *prop, 
					prefHandle, object, isGlobalProperty, errors);
			}
		}

		if (!errors.empty())
		{
			NGT_ERROR_MSG("Errors occured while loading %s global settings: %s", id, errors.c_str());
		}

		grabber->onPostPreferencesLoaded();
	}

	void GrabberManager::Implementation::saveGlobalSettings(IGrabber* grabber)
	{
		auto preferences = get<IPreferences>();
		auto definitionManager = get<IDefinitionManager>();
		if(!preferences || !definitionManager)
		{
			return;
		}

		auto object = grabber->getPropertiesObject();
		auto definition = definitionManager->getDefinition(object);
		if (!definition)
		{
			return;
		}

		const auto& unique = grabber->getUniquePropertyNames();
		auto isGlobalProperty = [&unique](const char* path) -> bool
		{
			return unique.empty() || 
				std::find(unique.begin(), unique.end(), path) == unique.end();
		};

		grabber->onPrePreferencesSaved();

		std::string errors;
		int prefCount = 0;
		auto prefObj = GenericObject::create();
		const char* id = grabber->getId();

		auto properties = definition->allProperties();
		for (const auto& property : properties)
		{
			prefCount += PreferenceUtils::savePropertyAsGeneric(
				*definitionManager, *definition, *property, 
				prefObj, object, isGlobalProperty, errors);
		}

		if (prefCount > 0)
		{
			auto& prefFile = preferences->getPreference(GRABBER_MANAGER_COMPONENT_ID);
			PreferenceUtils::serialize(*definitionManager, id, prefFile, prefObj, errors);
		}

		if (!errors.empty())
		{
			NGT_ERROR_MSG("Errors occured while saving %s global settings: %s", id, errors.c_str());
		}
	}

	void GrabberManager::Implementation::saveEnvironmentSettings(const EnvironmentId& envId)
	{
		auto preferences = get<IPreferences>();
		auto envManager = get<IEnvManager>();
		auto definitionManager = get<IDefinitionManager>();
		if (!preferences || !envManager || !definitionManager)
		{
			return;
		}

		const auto name = envManager->getEnvironmentPreferenceFileName(envId);
		if(name.empty())
		{
			return;
		}

		std::string errors;
		auto& prefFile = preferences->getPreference(GRABBER_MANAGER_COMPONENT_ID, name);
		auto activeEnvId = envManager->getActiveEnvironmentId();
		{
			std::lock_guard<std::mutex> lock(grabbersMutex_);
			for (auto& grabber : grabbers_)
			{
				auto& props = grabber.second.uniqueProperties_;
				if(props.empty())
				{
					continue;
				}

				auto& object = grabber.second.handle_;
				auto definition = definitionManager->getDefinition(object);
				if (!definition)
				{
					return;
				}

				if(activeEnvId != envId)
				{
					restoreEnvironmentSettings(grabber.second, envId);
				}

				int prefCount = 0;
				auto prefObj = GenericObject::create();
				for (size_t i = 0; i < props.size(); ++i)
				{
					prefCount += PreferenceUtils::savePropertyAsGeneric(
						*definitionManager, *definition, *props[i], 
						prefObj, object, nullptr, errors);
				}

				if(prefCount > 0)
				{
					PreferenceUtils::serialize(*definitionManager, 
						grabber.first->getId(), prefFile, prefObj, errors);
				}

				if(activeEnvId != envId)
				{
					restoreEnvironmentSettings(grabber.second, activeEnvId);
				}
			}
		}

		if(!errors.empty())
		{
			NGT_ERROR_MSG("Errors occured while saving %s project settings: %s", 
				name.c_str(), errors.c_str());
		}
	}

	void GrabberManager::Implementation::loadEnvironmentSettings(const EnvironmentId& envId)
	{
		auto preferences = get<IPreferences>();
		auto envManager = get<IEnvManager>();
		auto definitionManager = get<IDefinitionManager>();
		if (!preferences || !envManager || !definitionManager)
		{
			return;
		}

		const auto name = envManager->getEnvironmentPreferenceFileName(envId);
		if (name.empty())
		{
			return;
		}

		if (!preferences->preferenceExists(GRABBER_MANAGER_COMPONENT_ID, name))
		{
			return;
		}

		std::string errors(!loadedSettingsDefinitions_ ?
			"\nSettings definitions have not been correctly loaded" : "");
		auto& prefFile = preferences->getPreference(GRABBER_MANAGER_COMPONENT_ID, name);
		auto activeEnvId = envManager->getActiveEnvironmentId();
		{
			std::lock_guard<std::mutex> lock(grabbersMutex_);
			for (auto& grabber : grabbers_)
			{
				auto& props = grabber.second.uniqueProperties_;
				if (props.empty())
				{
					continue;
				}

				const char* id = grabber.first->getId();
				if (!prefFile->findProperty(id).isValid())
				{
					continue;
				}

				auto& object = grabber.second.handle_;
				auto definition = definitionManager->getDefinition(object);
				if (!definition)
				{
					return;
				}

				auto prefObj = GenericObject::create();
				if (!PreferenceUtils::deserialize(*definitionManager, id, prefFile, prefObj, errors))
				{
					continue;
				}

				if (activeEnvId != envId)
				{
					restoreEnvironmentSettings(grabber.second, envId);
				}

				int prefCount = 0;
				auto prefHandle = prefObj.getHandleT();
				for (size_t i = 0; i < props.size(); ++i)
				{
					prefCount += PreferenceUtils::loadPropertyFromGeneric(
						*definitionManager, *definition, *props[i],
						prefHandle, object, nullptr, errors);
				}

				if (prefCount > 0)
				{
					storeEnvironmentSettings(grabber.second, envId);
				}

				if (activeEnvId != envId)
				{
					restoreEnvironmentSettings(grabber.second, activeEnvId);
				}
			}
		}

		if (!errors.empty())
		{
			NGT_ERROR_MSG("Errors occured while loading %s project settings: %s",
				name.c_str(), errors.c_str());
		}
	}

	void GrabberManager::Implementation::onEnvironmentRemoved(const EnvironmentId& envId)
	{
		saveEnvironmentSettings(envId);
		{
			std::lock_guard<std::mutex> lock(grabbersMutex_);
			for (auto& grabber : grabbers_)
			{
				grabber.second.uniqueValues_.erase(envId);
			}
		}
	}

	void GrabberManager::Implementation::onEnvironmentCreated(const EnvironmentId& envId)
	{
		{
			std::lock_guard<std::mutex> lock(grabbersMutex_);
			for (auto& grabber : grabbers_)
			{
				auto& values = grabber.second.uniqueValues_[envId];
				auto& props = grabber.second.uniqueProperties_;

				// The properties don't already exist, create them now
				values.resize(props.size());
				for (size_t i = 0; i < props.size(); ++i)
				{
					values[i] = grabber.second.defaultValues_[i];
				}
			}
		}
		loadEnvironmentSettings(envId);
	}

	void GrabberManager::Implementation::onPreEnvironmentChanged(const EnvironmentId& oldId, const EnvironmentId& newId)
	{
		std::lock_guard<std::mutex> lock(grabbersMutex_);

		// Swap values
		for (auto itr = grabbers_.begin(), end = grabbers_.end(); itr != end; ++itr)
		{
			IGrabber* grabber = itr->first;
			if (!itr->second.initialized_)
			{
				// Try and initialise the object now
				registerGrabber(grabber);
				if (!itr->second.initialized_)
				{
					continue;
				}
			}

			std::vector<IBasePropertyPtr>& props = itr->second.uniqueProperties_;
			ObjectHandle& grabberObject = grabber->getPropertiesObject();
			auto definitionManager = get<IDefinitionManager>();

			// Save the old values to the environment
			if(itr->second.initialized_ && grabberObject != nullptr)
			{
				// Ensure it isn't an environment that has been removed
				auto valuesItr = itr->second.uniqueValues_.find(oldId);
				if (valuesItr != itr->second.uniqueValues_.end())
				{
					storeEnvironmentSettings(itr->second, oldId);
				}
			}

			auto& values = itr->second.uniqueValues_;
			values[newId].resize(itr->second.uniqueProperties_.size());

			grabber->onPreEnvironmentChanged(newId);
		}
	}

	void GrabberManager::Implementation::onPostEnvironmentChanged(const EnvironmentId& oldId, const EnvironmentId& newId)
	{
		std::lock_guard<std::mutex> lock(grabbersMutex_);

		for(auto itr = grabbers_.begin(), end = grabbers_.end(); itr != end; ++itr)
		{
			IGrabber* grabber = itr->first;
			if (!itr->second.initialized_)
			{
				continue;
			}

			grabber->onPostEnvironmentChanged(newId);

			const ObjectHandle postHandle = grabber->getPropertiesObject();
			const ObjectHandle& prevHandle = itr->second.handle_;
			if(postHandle != prevHandle)
			{
				// Object handle has changed in this environment, update it
				assert(itr->second.actions_.size() == 0 && "Grabbers with actions cannot currently \
															have their object changed!");
				settingsContainer_->set(grabber->getTitle(), postHandle);
				itr->second.handle_ = postHandle;
			}

			setGrabberVisibility(*grabber, itr->second, grabber->isSupportedEnvironment(newId));

			restoreEnvironmentSettings(itr->second, newId);

			grabber->onPostSettingsChanged();
		}

		updatePanel();
	}

	void GrabberManager::Implementation::setGrabberVisibility(IGrabber& grabber, GrabberDetails& details, bool visible)
	{
		if(details.visible_ != visible)
		{
			details.visible_ = visible;
			for (auto& action : details.actions_)
			{
				action->visible(visible);
			}
		}
	}

	void GrabberManager::Implementation::updatePanel()
	{
		// Currently only way to do this is to set the object
		settingsViewModel_->setObject(settingsContainer_.getHandle());
		refresh_ = false;
	}

	void GrabberManager::Implementation::createPanel()
	{
		auto viewCreator = get<IViewCreator>();
		if(viewCreator == nullptr)
		{
			return;
		}

		settingsViewModel_ = CreatePropertyViewModel<proto::PropertyTreeModel>();
		settingsViewModel_->setTitle("Settings");
		settingsViewModel_->addLayoutHint("right", 1.0f);
		settingsViewModel_->setObject(settingsContainer_.getHandle());

		const char* panelResource = "WGControls/WGPropertyPanel.qml";
		std::string panelName = std::string(panelResource) + "SettingsNew";
		settingsPanel_ = viewCreator->createView(panelResource, settingsViewModel_.getHandle(), panelName.c_str());
	}

	void GrabberManager::Implementation::destroyPanel()
	{
		auto uiApplication = get<IUIApplication>();
		if(uiApplication == nullptr)
		{
			return;
		}
		auto view = settingsPanel_.get();
		uiApplication->removeView(*view);
		view = nullptr;
	}

	void GrabberManager::Implementation::postLoad()
	{
		settingsContainer_ = GenericObject::create();
	}

	void GrabberManager::Implementation::init()
	{
		this->createPanel();
	}

	void GrabberManager::Implementation::fini()
	{
		this->destroyPanel();
	}

	void GrabberManager::Implementation::addActions(IGrabber* grabber, std::function<bool()> iconVisibleFn)
	{
		auto definitionManager = get<IDefinitionManager>();

		if (!definitionManager)
		{
			return;
		}

		auto& details = grabbers_[grabber];
		auto object = grabber->getPropertiesObject();
		auto definition = definitionManager->getDefinition(object);
		auto properties = definition->allProperties();
		auto propertiesBegin = properties.begin();
		auto propertiesEnd = properties.end();

		for (auto itr = propertiesBegin; itr != propertiesEnd; ++itr)
		{
			addAction(grabber, iconVisibleFn, object, *itr);
		}
	}

	void GrabberManager::Implementation::addAction(IGrabber* grabber, std::function<bool()> iconVisibleFn, 
		                                                               ObjectHandle object, 
		                                                               IBasePropertyPtr property)
	{
		auto app = get<IUIApplication>();
		if(app == nullptr)
		{
			return;
		}

		auto& details = grabbers_[grabber];
		auto definitionManager = get<IDefinitionManager>();
		auto metaAutoAction = findFirstMetaData<MetaAutoActionObj>(*property, *definitionManager);
		if (!metaAutoAction.get())
		{
			return;
		}

		auto metaReadOnly = findFirstMetaData<MetaReadOnlyObj>(*property, *definitionManager);
		auto metaAction   = findFirstMetaData<MetaActionObj>(*property, *definitionManager);
		auto metaEnum   = findFirstMetaData<MetaEnumObj>(*property, *definitionManager);
		std::string objectPath = object.path();
		std::string propertyName = property->getName();
		std::string path = objectPath.empty() ? propertyName : objectPath + "." + propertyName;
		std::string id = metaAutoAction->getActionId();

		std::function<void(IAction*)> callback = nullptr;
		std::function<bool(const IAction*)> isEnabled = [](const IAction*) { return true; };
		std::function<bool(const IAction*)> isChecked = nullptr;
		std::function<bool(const IAction*)> isVisible = [iconVisibleFn](const IAction*) {
			return iconVisibleFn ? iconVisibleFn() : true;
		};

		if(metaReadOnly != nullptr)
		{
			isEnabled = [metaReadOnly, object](const IAction* action) -> bool
			{
				return !metaReadOnly->isReadOnly(object);
			};
		}
		
		if (metaAction != nullptr)
		{
			callback = [this, metaAction, object, path](IAction* action)
			{
				metaAction->execute(object, path);
				updatePanel();
			};
		}
		else if (metaEnum != nullptr)
		{
			auto collection = metaEnum->generateEnum(object);
			for (auto it = collection.begin(); it != collection.end(); ++it)
			{
				auto enumName = it.value().value<std::string>();
				auto enumValue = it.key().value<int>();
				callback = [this, object, path, enumValue](IAction* action)
				{
					IDefinitionManager* definitionManager = get<IDefinitionManager>();
					auto definition = definitionManager->getDefinition(object);
					auto accessor = definition->bindProperty(path.c_str(), object);
					accessor.setValue(enumValue);
					updatePanel();
				};
				isChecked = [this, object, property, enumValue, enumName](const IAction* action) -> bool
				{
					IDefinitionManager* definitionManager = get<IDefinitionManager>();
					Variant value = property->get(object, *definitionManager);
					return value == enumValue || value == enumName;
				};
				auto optionPath = id + "." + enumName;
				auto action = get<IUIFramework>()->createAction(optionPath.c_str(), callback, isEnabled, isChecked, isVisible);
				app->addAction(*action);
				details.actions_.push_back(std::move(action));
			}
			return;
		}
		else if (property->getType() == TypeId::getType<bool>())
		{
			callback = [this, object, path](IAction* action)
			{
				IDefinitionManager* definitionManager = get<IDefinitionManager>();
				auto definition = definitionManager->getDefinition(object);
				auto accessor = definition->bindProperty(path.c_str(), object);
				auto value = accessor.getValue().cast<bool>();
				accessor.setValue(!value);
				updatePanel();
			};

			isChecked = [this, object, property](const IAction* action) -> bool
			{
				IDefinitionManager* definitionManager = get<IDefinitionManager>();
				Variant value = property->get(object, *definitionManager);
				return value.cast<bool>();
			};
		}

		auto action = get<IUIFramework>()->createAction(id.c_str(), callback, isEnabled, isChecked, isVisible);
		app->addAction(*action);
		details.actions_.push_back(std::move(action));
	}

	void GrabberManager::Implementation::removeActions(IGrabber* grabber)
	{
		auto app = get<IUIApplication>();
		auto& grabberDetail = grabbers_[grabber];

		for (size_t i = 0; i < grabberDetail.actions_.size(); ++i)
		{
			auto action = grabberDetail.actions_[i].get();
			app->removeAction(*action);
		}
	}

	bool GrabberManager::Implementation::registerGrabber(IGrabber* grabber, std::function<bool()> iconVisibleFn)
	{
		std::lock_guard<std::mutex> lock(grabbersMutex_);

		if(grabbers_[grabber].initialized_)
		{
			return true;
		}

		// Get grabber object and definitionManager
		ObjectHandle propertyObject = grabber->getPropertiesObject();
		if(propertyObject == nullptr)
		{
			return false;
		}

		IDefinitionManager* definitionManager = get<IDefinitionManager>();
		auto definition = definitionManager->getDefinition(propertyObject);
		if(definition == nullptr)
		{
			return false;
		}

		GrabberDetails& grabberDetails = grabbers_[grabber];

		// Get the unique property names
		const auto& uniquePropertyNames = grabber->getUniquePropertyNames();
		std::vector<IBasePropertyPtr>& settingUniqueProperties = grabberDetails.uniqueProperties_;

		// Add the unique properties to this setting's property collection
		for(auto iter = uniquePropertyNames.begin(), end = uniquePropertyNames.end(); iter != end; ++iter)
		{
			auto prop = definition->findProperty(iter->c_str());
			settingUniqueProperties.push_back(prop);
		}

		// Set default values
		std::vector<Variant>& defaults = grabberDetails.defaultValues_;
		defaults.resize(uniquePropertyNames.size());

		for(size_t i = 0; i < settingUniqueProperties.size(); ++i)
		{
			defaults[i] = settingUniqueProperties[i]->get(propertyObject, *definitionManager);
		}

		// Override with any saved global preferences
		loadGlobalSettings(grabber, grabberDetails);

		// Get the current environment id
		EnvironmentId currentEnvId = get<IEnvManager>()->getActiveEnvironmentId();

		// Create some values for this environment
		grabberDetails.uniqueValues_[currentEnvId] = defaults;

		addActions(grabber, iconVisibleFn);

		grabberDetails.handle_ = grabber->getPropertiesObject();
		grabberDetails.initialized_ = true;

		// Let the grabber know about the current environment
		grabber->onPostEnvironmentChanged(currentEnvId);
		setGrabberVisibility(*grabber, grabberDetails, grabber->isSupportedEnvironment(currentEnvId));

		// Add to the settings container
		settingsContainer_->add(grabber->getTitle(), Variant(grabberDetails.handle_), MetaHidden(
			[&grabberDetails](const ObjectHandle&){ return !grabberDetails.visible_; }));

		return true;
	}

	bool GrabberManager::Implementation::deregisterGrabber(IGrabber* grabber)
	{
		std::lock_guard<std::mutex> lock(grabbersMutex_);

		auto itr = grabbers_.find(grabber);
		if (itr != grabbers_.end())
		{
			saveGlobalSettings(grabber);
			settingsContainer_->set(grabber->getTitle(), Variant());
			removeActions(grabber);
			grabbers_.erase(itr);
		}
		return true;
	}

	GrabberManager::GrabberManager()
		: impl_(std::make_unique<Implementation>(*this))
	{
	}

	GrabberManager::~GrabberManager()
	{
	}

	void GrabberManager::registerGrabber(IGrabber* grabber, std::function<bool()> iconVisibleFn)
	{
		if(grabber == nullptr)
		{
			return;
		}
		impl_->registerGrabber(grabber, iconVisibleFn);
	}

	void GrabberManager::deregisterGrabber(IGrabber* grabber)
	{
		if(grabber == nullptr)
		{
			return;
		}
		impl_->deregisterGrabber(grabber);
	}

	void GrabberManager::refreshGrabber(IGrabber* grabber)
	{
		impl_->refresh_ = true;
	}

	void GrabberManager::postDraw()
	{
		std::lock_guard<std::mutex> lock(impl_->grabbersMutex_);

		for (auto& grabber : impl_->grabbers_)
		{
			if(grabber.second.visible_)
			{
				grabber.first->postDraw();
			}
		}
	}

	void GrabberManager::draw()
	{
		std::lock_guard<std::mutex> lock(impl_->grabbersMutex_);

		for (auto& grabber: impl_->grabbers_)
		{
			if(grabber.second.visible_)
			{
				grabber.first->draw();
			}
		}
	}

	void GrabberManager::update()
	{
		std::lock_guard<std::mutex> lock(impl_->grabbersMutex_);

		for (auto& grabber: impl_->grabbers_)
		{
			if(grabber.second.visible_)
			{
				if (!grabber.second.initialized_)
				{
					impl_->registerGrabber(grabber.first);
				}
				grabber.first->update();
			}
		}

		if(impl_->refresh_)
		{
			impl_->updatePanel();
		}
	}

	void GrabberManager::onMouseFocusChanged(bool leavingFocus)
	{
		std::lock_guard<std::mutex> lock(impl_->grabbersMutex_);

		for (auto& grabber : impl_->grabbers_)
		{
			if(grabber.second.visible_)
			{
				grabber.first->onMouseFocusChanged(leavingFocus);
			}
		}
	}

	bool GrabberManager::onMouseButtonDown(MouseButton button, int x, int y)
	{
		std::lock_guard<std::mutex> lock(impl_->grabbersMutex_);

		bool hadAction = false;
		for (auto& grabber : impl_->grabbers_)
		{
			if(grabber.second.visible_)
			{
				hadAction |= grabber.first->onMouseButtonDown(button, x, y);
			}
		}
		return hadAction;
	}

	bool GrabberManager::onMouseButtonUp(MouseButton button, int x, int y)
	{
		std::lock_guard<std::mutex> lock(impl_->grabbersMutex_);

		bool hadAction = false;
		for (auto& grabber : impl_->grabbers_)
		{
			if(grabber.second.visible_)
			{
				hadAction |= grabber.first->onMouseButtonUp(button, x, y);
			}
		}
		return hadAction;
	}

	bool GrabberManager::onMouseMove(int x, int y)
	{
		std::lock_guard<std::mutex> lock(impl_->grabbersMutex_);

		bool hadAction = false;
		for (auto& grabber : impl_->grabbers_)
		{
			if(grabber.second.visible_)
			{
				hadAction |= grabber.first->onMouseMove(x, y);
			}
		}
		return hadAction;
	}

	void GrabberManager::postLoad()
	{
		impl_->postLoad();
	}

	void GrabberManager::init()
	{
		impl_->init();
	}

	void GrabberManager::fini()
	{
		impl_->fini();
	}

} // end namespace wgt