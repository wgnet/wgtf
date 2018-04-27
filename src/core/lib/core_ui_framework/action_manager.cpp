#include "action_manager.hpp"

#include "core_common/assert.hpp"
#include "core_logging/logging.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/i_window.hpp"
#include "core_serialization_xml/simple_api_for_xml.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_ui_framework/i_preferences.hpp"
#include "core_variant/collection.hpp"
#include "core_data_model/collection_model.hpp"
#include "core_reflection/type_class_definition.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/function_property.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/utilities/reflection_function_utilities.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"
#include "private/shortcut_dialog_model.hpp"
#include "wg_types/base64.hpp"
#include <algorithm>
#include <unordered_map>

namespace wgt
{
namespace
{
const std::string ROOT = "Actions";
const std::string TEXT = "text";
const std::string ICON = "icon";
const std::string WINDOWID = "windowId";
const std::string PATH = "path";
const std::string SHORTCUT = "shortcut";
const std::string ORDER = "order";
const std::string SEPARATOR = "separator";
const std::string EXCLUSIVE_GROUP = "group";
const std::string MENU_ITEM = "Menu";
const std::string NAME = "name";

const std::string preferenceId = "0D18FE37-ED9E-473D-8878-3EB4CB8BF916";

}
int ActionManager::s_defaultActionOrder = 0;

struct MenuNameData
{
	std::string name;
	std::string windowId;
};

struct ActionManager::Impl
{
	struct ActionData
	{
		std::string id_;
		std::string text_;
		std::string icon_;
		std::string windowId_;
		std::string path_;
		std::string shortcut_;
		std::string group_;
		int order_;
		bool isSeparator_;
	};

	class ActionsReader : private SimpleApiForXml
	{
		typedef SimpleApiForXml base;

	public:
		ActionsReader(TextStream& stream, std::unordered_map<std::string, std::unique_ptr<ActionData>>& actionData,
		              std::vector<MenuNameData>& menuNames)
		    : base(stream), actionData_(actionData), menuNames_(menuNames)
		{
		}
		bool read()
		{
			return parse();
		}
		virtual void elementStart(const char* elementName, const char* const* attributes) override
		{
			if (elementName == ROOT)
			{
				return;
			}

			if (elementName == MENU_ITEM)
			{
				const char* attributeName = attributes[0];
				const char* attributeValue = attributes[1];
				MenuNameData menuName;

				for (auto attribute = attributes; *attribute; attribute += 2)
				{
					if (attributeName == NAME)
					{
						menuName.name = attributeValue;
					}
					else if (attributeName == WINDOWID)
					{
						menuName.windowId = attributeValue;
					}
				}

				menuNames_.push_back(menuName);
				return;
			}

			std::string id(elementName);
			ActionData* actionData = nullptr;
			auto it = actionData_.find(id);
			if (it == actionData_.end())
			{
				actionData = new ActionData;
				actionData_[id] = std::unique_ptr<ActionData>(actionData);
			}
			else
			{
				actionData = it->second.get();
			}
			actionData->id_ = elementName;
			actionData->order_ = 0;
			actionData->isSeparator_ = false;
			std::string tmp;
			// parse attributes
			for (auto attribute = attributes; *attribute; attribute += 2)
			{
				const char* attributeName = attribute[0];
				const char* attributeValue = attribute[1];

				if (attributeName == TEXT)
				{
					actionData->text_ = attributeValue;
				}
				else if (attributeName == ICON)
				{
					actionData->icon_ = attributeValue;
				}
				else if (attributeName == WINDOWID)
				{
					actionData->windowId_ = attributeValue;
				}
				else if (attributeName == PATH)
				{
					actionData->path_ = attributeValue;
				}
				else if (attributeName == SHORTCUT)
				{
					actionData->shortcut_ = attributeValue;
				}
				else if (attributeName == ORDER)
				{
					actionData->order_ = strtol(attributeValue, nullptr, 0);
				}
				else if (attributeName == SEPARATOR)
				{
					tmp = attributeValue;
					std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
					if (tmp == "true")
					{
						actionData->isSeparator_ = true;
					}
				}
				else if (attributeName == EXCLUSIVE_GROUP)
				{
					actionData->group_ = attributeValue;
				}
				else
				{
					// ignore unknown attributes
					NGT_WARNING_MSG("Warning: unknown action data attribute \"%s\".\n", attributeName);
				}
			}
		}

	private:
		std::unordered_map<std::string, std::unique_ptr<ActionData>>& actionData_;
		std::vector<MenuNameData>& menuNames_;
	};

	Impl(ActionManager& self) : self_(self), currentShortcutCollection_(currentActionShortcuts_)
	{
		shortcutModel_ = ManagedObject<ShortcutDialogModel>::make();
		shortcutModel_->init(currentShortcutCollection_);
	}
	~Impl()
	{
	}

	void init()
	{
		// TODO: uncomments follow code could help to reproduce issue: NGT-3408
		// auto framework = self_.get<IUIFramework>();
		// TF_ASSERT(framework != nullptr);
		// auto preferences = framework->getPreferences();
		// if (preferences == nullptr)
		//{
		//	return;
		//}
		// auto preference = preferences->getPreference(preferenceId.c_str());
		// TF_ASSERT(preference != nullptr);

		// auto definition = preference.getDefinition(*definitionManager);
		// auto properties = definition->allProperties();
		// auto it = properties.begin();
		// std::string key;
		// std::string value;
		// std::string encodedShortcut;
		// for (; it != properties.end(); ++it)
		//{
		//	auto id = it->getName();
		//	preference->get(id, encodedShortcut);
		//	bool isOk = Base64::decode(std::string(id), key);
		//	TF_ASSERT(isOk);
		//	isOk = Base64::decode(encodedShortcut, value);
		//	TF_ASSERT(isOk);
		//	currentShortcutCollection_[key] = value;
		//}
	}

	void fini()
	{
		shortcutModel_->fini();

		connections_.clear();
		auto framework = self_.get<IUIFramework>();
		TF_ASSERT(framework != nullptr);
		auto preferences = framework->getPreferences();
		if (preferences == nullptr)
		{
			return;
		}
		auto preference = preferences->getPreference(preferenceId.c_str());
		TF_ASSERT(preference != nullptr);
		std::string key;
		auto& defaultActionShortcuts = shortcutModel_->getDefaultActionShortcuts();
		for (auto& it : currentActionShortcuts_)
		{
			// Don't save default shortcuts allows them to be changed in the future
			auto findIt = defaultActionShortcuts.find(it.first);
			if (findIt != defaultActionShortcuts.end())
			{
				if (findIt->second == it.second)
				{
					//remove the override shortcut in preference if we reset it to default value
					preference->set(Base64::encode(it.first.c_str(), it.first.length()).c_str(),
						Variant());
					continue;
				}
			}
			preference->set(Base64::encode(it.first.c_str(), it.first.length()).c_str(),
			                Base64::encode(it.second.c_str(), it.second.length()));
		}
		for (auto&& it : actionsSeparators_)
		{
			auto application = self_.get<IUIApplication>();
			TF_ASSERT(application != nullptr);
			application->removeAction(*it.second);
		}
		actionsSeparators_.clear();

        shortcutModel_ = nullptr;
	}

	void loadActionData(TextStream& stream)
	{
		ActionsReader actions(stream, defaultActionData_, defaultMenuNames_);
		if (!actions.read())
		{
			NGT_ERROR_MSG("Failed to parse actions\n");
			return;
		}
		auto application = self_.get<IUIApplication>();
		TF_ASSERT(application != nullptr);

		for (auto&& it : defaultMenuNames_)
		{
			application->addMenuPath(it.name.c_str(), it.windowId.c_str());
		}
		for (auto&& it : defaultActionData_)
		{
			auto& actionData = it.second;
			const char* id = it.first.c_str();
			auto tok = strrchr(id, ':');
			auto order = tok != nullptr ? strtol(tok + 1, nullptr, 0) : s_defaultActionOrder;
			auto fullPath = tok != nullptr ? std::string(id, tok - id) : id;
			id = fullPath.c_str();
			tok = strrchr(id, '.');
			auto path = tok != nullptr ? std::string(id, tok - id) : "";
			auto findIt = actionsSeparators_.find(id);
			if (findIt != actionsSeparators_.end())
			{
				continue;
			}
			if (actionData->isSeparator_)
			{
				auto action = self_.createAction(id,
				actionData->text_.c_str(), actionData->icon_.c_str(), actionData->windowId_.c_str(),
				actionData->path_.empty() ? path.c_str() : actionData->path_.c_str(), "",
				actionData->order_ == s_defaultActionOrder ? order : actionData->order_, [](IAction*) {},
				[](const IAction*) { return true; }, std::function<bool(const IAction*)>(nullptr), 
				[](const IAction*) { return true; }, "", true);

				application->addAction(*action);
				actionsSeparators_[id] = std::move(action);
			}
		}
	}

	bool registerActionData(const char* id, std::unique_ptr<ActionData>& actionData)
	{
		auto it = defaultActionData_.find(id);
		if (it != defaultActionData_.end())
		{
			return false;
		}

		defaultActionData_[id] = std::unique_ptr<ActionData>(actionData.release());
		return true;
	}

	bool getShortcutPreference(const char* id, std::string& o_shortcut)
	{
		auto definitionManager = self_.get<IDefinitionManager>();
		TF_ASSERT(definitionManager != nullptr);
		auto framework = self_.get<IUIFramework>();
		TF_ASSERT(framework != nullptr);
		auto preferences = framework->getPreferences();
		if (preferences == nullptr)
		{
			return false;
		}
		auto preference = preferences->getPreference(preferenceId.c_str());
		TF_ASSERT(preference != nullptr);

		auto definition = definitionManager->getDefinition(preference);
		auto properties = definition->allProperties();
		auto it = properties.begin();
		std::string shortcutId;
		for (; it != properties.end(); ++it)
		{
			auto key = it->getName();
			bool isOk = Base64::decode(std::string(key), shortcutId);
			TF_ASSERT(isOk);
			if (shortcutId == id)
			{
				std::string encodedShortcut;
				preference->get(key, encodedShortcut);
				isOk = Base64::decode(encodedShortcut, o_shortcut);
				TF_ASSERT(isOk);
				return true;
			}
		}
		return false;
	}

	std::unique_ptr<IAction> createAction(const char* id, const char* text, const char* path, int actionOrder,
	                                      std::function<void(IAction*)> func,
	                                      std::function<bool(const IAction*)> enableFunc,
	                                      std::function<bool(const IAction*)> checkedFunc,
										  std::function<bool(const IAction*)> visibleFunc)
	{
		std::unique_ptr<IAction> action;

		auto order = actionOrder;
		std::string shortcut("");
		bool hasUserShortcut = getShortcutPreference(id, shortcut);

		// Attempt to find action data for the passed in id
		auto it = defaultActionData_.find(id);
		if (it == defaultActionData_.end())
		{
			it = defaultActionData_.find(path);
		}

		if (it != defaultActionData_.end())
		{
			auto& actionData = *it->second;

			auto& defaultActionShortcuts = shortcutModel_->getDefaultActionShortcuts();
			if (actionData.isSeparator_)
			{
				return nullptr;
			}
			defaultActionShortcuts[id] = actionData.shortcut_;
			currentShortcutCollection_.insertValue(id, hasUserShortcut ? shortcut : actionData.shortcut_);

			action =
			self_.createAction(id, actionData.text_.empty() && !actionData.isSeparator_ ? text : actionData.text_.c_str(),
			                   actionData.icon_.c_str(), actionData.windowId_.c_str(),
			                   actionData.path_.empty() ? path : actionData.path_.c_str(),
			                   hasUserShortcut ? shortcut.c_str() : actionData.shortcut_.c_str(),
			                   actionData.order_ == s_defaultActionOrder ? order : actionData.order_, func, enableFunc,
			                   checkedFunc, visibleFunc, actionData.group_.c_str(), func == nullptr);
		}
		else
		{
			auto& defaultActionShortcuts = shortcutModel_->getDefaultActionShortcuts();
			defaultActionShortcuts[id] = "";
			currentShortcutCollection_.insertValue(id, shortcut);

			// Fall back to creating an action with the passed in text and path
			bool isSeparator = func == nullptr;
			action = self_.createAction(id, text, "", "", path, shortcut.c_str(), order,
				func, enableFunc, checkedFunc, visibleFunc, "", isSeparator);
		}

		TF_ASSERT(action != nullptr);
		auto pAction = action.get();
		actions_[id] = pAction;
		return action;
	}

	std::unique_ptr<IAction> createAction(const char* id, std::function<void(IAction*)> func,
	                                      std::function<bool(const IAction*)> enableFunc,
	                                      std::function<bool(const IAction*)> checkedFunc,
										  std::function<bool(const IAction*)> visibleFunc)
	{
		auto tok = strrchr(id, ':');
		auto order = tok != nullptr ? strtol(tok + 1, nullptr, 0) : s_defaultActionOrder;
		auto fullPath = tok != nullptr ? std::string(id, tok - id) : id;

		id = fullPath.c_str();
		tok = strrchr(id, '.');
		auto text = tok != nullptr ? tok + 1 : fullPath;
		auto path = tok != nullptr ? std::string(id, tok - id) : "";

		return createAction(id, text.c_str(), path.c_str(), order, func, enableFunc, checkedFunc, visibleFunc);
	}

	void removeAction(IAction* action)
	{
		for (auto& it : actions_)
		{
			if (it.second == action)
			{
				actions_.erase(it.first);
				return;
			}
		}
	}

	void registerEventHandler(IWindow* window)
	{
		TF_ASSERT(window != nullptr);
		connections_ += window->signalClose.connect(std::bind(&Impl::onShortcutDialogClosed, this));
	}

	void onShortcutDialogClosed()
	{
		shortcutModel_->onDialogClosed(currentActionShortcuts_, [&](bool updateNeeded) {
			if (!updateNeeded)
			{
				return;
			}
			bool changed = false;
			std::string key;
			std::string value;
			for (auto& it : currentActionShortcuts_)
			{
				key = it.first;
				value = it.second;
				auto&& findIt = actions_.find(key);
				if (findIt == actions_.end())
				{
					continue;
				}
				auto action = findIt->second;
				TF_ASSERT(action != nullptr);
				if (value != action->shortcut())
				{
					action->setShortcut(value.c_str());
					changed = true;
				}
			}
			if (changed)
			{
				auto framework = self_.get<IUIFramework>();
				TF_ASSERT(framework != nullptr);
				framework->signalKeyBindingsChanged();
			}
		});
	}

	ActionManager& self_;
	std::unordered_map<std::string, std::unique_ptr<ActionData>> defaultActionData_;
	std::vector<MenuNameData> defaultMenuNames_;
	Collection currentShortcutCollection_;
	std::unordered_map<std::string, std::string> currentActionShortcuts_;
	ManagedObject<ShortcutDialogModel> shortcutModel_;
	std::unordered_map<std::string, IAction*> actions_;
	std::unordered_map<std::string, std::unique_ptr<IAction>> actionsSeparators_;
	ConnectionHolder connections_;
};

ActionManager::ActionManager() : impl_(new Impl(*this))
{
}

ActionManager::~ActionManager()
{
}

void ActionManager::init()
{
	impl_->init();
}

void ActionManager::fini()
{
	impl_->fini();
}

IAction* ActionManager::findAction(const char* id) const
{
	return impl_->actions_.find(id) != impl_->actions_.end() ? impl_->actions_.at(id) : nullptr;
}

std::unique_ptr<IAction> ActionManager::createSeperator(const char* id)
{
	return impl_->createAction(id,
		nullptr,
		[](const IAction*) { return true; },
		std::function<bool(const IAction*)>(nullptr),
		[](const IAction*) { return true; });
}

std::unique_ptr<IAction> ActionManager::createSeperator(const char* id, const char* path, int actionOrder)
{
	return impl_->createAction(id, "", path, actionOrder,
		nullptr,
		[](const IAction*) { return true; },
		std::function<bool(const IAction*)>(nullptr),
		[](const IAction*) { return true; });
}

std::unique_ptr<IAction> ActionManager::createAction(const char* id, std::function<void(IAction*)> func,
                                                     std::function<bool(const IAction*)> enableFunc,
                                                     std::function<bool(const IAction*)> checkedFunc,
													 std::function<bool(const IAction*)> visibleFunc)
{
	return impl_->createAction(id, func, enableFunc, checkedFunc, visibleFunc);
}

std::unique_ptr<IAction> ActionManager::createAction(const char* id, const char* text, const char* path,
                                                     int actionOrder, std::function<void(IAction*)> func,
                                                     std::function<bool(const IAction*)> enableFunc,
                                                     std::function<bool(const IAction*)> checkedFunc,
													 std::function<bool(const IAction*)> visibleFunc)
{
	return impl_->createAction(id, text, path, actionOrder, func, enableFunc, checkedFunc, visibleFunc);
}

void ActionManager::loadActionData(IDataStream& dataStream)
{
	TextStream stream(dataStream);
	impl_->loadActionData(stream);
}

ObjectHandle ActionManager::getContextObject() const
{
    return impl_->shortcutModel_.getHandle();
}

void ActionManager::registerEventHandler(IWindow* window)
{
	impl_->registerEventHandler(window);
}

void ActionManager::onActionDestroyed(IAction* action)
{
	impl_->removeAction(action);
}
} // end namespace wgt
