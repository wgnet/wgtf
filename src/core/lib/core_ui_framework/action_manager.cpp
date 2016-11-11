#include "action_manager.hpp"
#include "core_logging/logging.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/i_window.hpp"
#include "core_serialization/serializer/details/simple_api_for_xml.hpp"
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

const std::string preferenceId = "0D18FE37-ED9E-473D-8878-3EB4CB8BF916";

class ShortcutDialogModel
{
	DECLARE_REFLECTED
public:
	ShortcutDialogModel() : applyChanges_(false), dirty_(false)
	{
	}
	~ShortcutDialogModel()
	{
	}
	void init(Collection& activeShortcuts)
	{
		connections_ += activeShortcuts.connectPostInserted([&](const Collection::Iterator& pos, size_t count)

		                                                    {
			                                                    std::string key;
			                                                    std::string value;
			                                                    bool isOk = false;
			                                                    Collection::Iterator iter = pos;
			                                                    for (size_t i = 0; i < count; i++, iter++)
			                                                    {
				                                                    isOk = iter.key().tryCast(key);
				                                                    assert(isOk);
				                                                    isOk = iter.value().tryCast(value);
				                                                    assert(isOk);
				                                                    shortcuts_[key] = value;
			                                                    }
			                                                });
		connections_ +=
		activeShortcuts.connectPostChanged([&](const Collection::Iterator& pos, const Variant& oldValue) {
			std::string key;
			std::string value;
			bool isOk = pos.key().tryCast(key);
			assert(isOk);
			isOk = pos.value().tryCast(value);
			assert(isOk);
			shortcuts_[key] = value;
		});
		Collection collection(shortcuts_);
		model_.setSource(collection);
		connections_ += model_.connectPostItemDataChanged(
		[&](int row, int column, ItemRole::Id role, const Variant& value) { dirty_ = true; });
	}

	void fini()
	{
		applyChanges_ = false;
		dirty_ = false;
		connections_.clear();
	}

	std::unordered_map<std::string, std::string>& getDefaultActionShortcuts()
	{
		return defaultActionShortcuts_;
	}

	const AbstractListModel* getModel() const
	{
		return &model_;
	}

	void applyChanges()
	{
		applyChanges_ = true;
	}

	void resetToDefault()
	{
		for (auto& it : shortcuts_)
		{
			auto findIt = defaultActionShortcuts_.find(it.first);
			if (findIt != defaultActionShortcuts_.end())
			{
				it.second = findIt->second;
			}
			else
			{
				it.second = "";
			}
		}
		dirty_ = true;
	}

	void onDialogClosed(std::unordered_map<std::string, std::string>& currentActionShortcut,
	                    std::function<void(bool updateNeeded)> callback = [](bool updateNeeded) {})
	{
		if (!applyChanges_)
		{
			shortcuts_ = currentActionShortcut;
		}
		else
		{
			if (dirty_)
			{
				currentActionShortcut = shortcuts_;
			}
			else
			{
				applyChanges_ = false;
			}
		}
		callback(applyChanges_);
		applyChanges_ = false;
		dirty_ = false;
	}

private:
	bool applyChanges_;
	bool dirty_;
	std::unordered_map<std::string, std::string> shortcuts_;
	std::unordered_map<std::string, std::string> defaultActionShortcuts_;
	CollectionModel model_;
	ConnectionHolder connections_;
};
}
BEGIN_EXPOSE(ShortcutDialogModel, MetaNone())
EXPOSE("shortcutModel", getModel, MetaNone())
EXPOSE_METHOD("applyChanges", applyChanges)
EXPOSE_METHOD("resetToDefault", resetToDefault)
END_EXPOSE()

int ActionManager::s_defaultActionOrder = 0;

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
		ActionsReader(TextStream& stream, std::unordered_map<std::string, std::unique_ptr<ActionData>>& actionData)
		    : base(stream), actionData_(actionData)
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
			std::string id(elementName);
			auto it = actionData_.find(id);
			if (it != actionData_.end())
			{
				NGT_WARNING_MSG("Warning: action \"%s\" already defined.\n", id.c_str());
				return;
			}
			auto actionData = new ActionData;
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
			actionData_[id] = std::unique_ptr<ActionData>(actionData);
		}

	private:
		std::unordered_map<std::string, std::unique_ptr<ActionData>>& actionData_;
	};

	Impl(ActionManager& self) : self_(self), currentShortcutCollection_(currentActionShortcuts_)
	{
	}
	~Impl()
	{
	}

	void init()
	{
		auto definitionManager = self_.get<IDefinitionManager>();
		assert(definitionManager != nullptr);
		shortcutModel_ = definitionManager->create<ShortcutDialogModel>();
		shortcutModel_->init(currentShortcutCollection_);
		auto framework = self_.get<IUIFramework>();
		assert(framework != nullptr);
		auto preferences = framework->getPreferences();
		if (preferences == nullptr)
		{
			return;
		}
		auto preference = preferences->getPreference(preferenceId.c_str());
		assert(preference != nullptr);

		auto definition = preference.getDefinition(*definitionManager);
		auto properties = definition->allProperties();
		auto it = properties.begin();
		std::string key;
		std::string value;
		std::string encodedShortcut;
		for (; it != properties.end(); ++it)
		{
			auto id = it->getName();
			preference->get(id, encodedShortcut);
			bool isOk = Base64::decode(std::string(id), key);
			assert(isOk);
			isOk = Base64::decode(encodedShortcut, value);
			assert(isOk);
			currentShortcutCollection_[key] = value;
		}
	}

	void fini()
	{
		shortcutModel_->fini();
		connections_.clear();
		auto framework = self_.get<IUIFramework>();
		assert(framework != nullptr);
		auto preferences = framework->getPreferences();
		if (preferences == nullptr)
		{
			return;
		}
		auto preference = preferences->getPreference(preferenceId.c_str());
		assert(preference != nullptr);
		std::string key;
		for (auto& it : currentActionShortcuts_)
		{
			preference->set(Base64::encode(it.first.c_str(), it.first.length()).c_str(),
			                Base64::encode(it.second.c_str(), it.second.length()));
		}
		for (auto&& it : actionsSeparators_)
		{
			auto application = self_.get<IUIApplication>();
			assert(application != nullptr);
			application->removeAction(*it.second);
		}
		actionsSeparators_.clear();
	}

	void loadActionData(TextStream& stream)
	{
		ActionsReader actions(stream, defaultActionData_);
		if (!actions.read())
		{
			NGT_ERROR_MSG("Failed to parse actions\n");
			return;
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
			auto text = tok != nullptr ? tok + 1 : fullPath;
			auto path = tok != nullptr ? std::string(id, tok - id) : "";
			auto findIt = actionsSeparators_.find(id);
			if (findIt != actionsSeparators_.end())
			{
				continue;
			}
			if (actionData->isSeparator_)
			{
				auto action = self_.createAction(
				actionData->text_.c_str(), actionData->icon_.c_str(), actionData->windowId_.c_str(),
				actionData->path_.empty() ? path.c_str() : actionData->path_.c_str(), "",
				actionData->order_ == s_defaultActionOrder ? order : actionData->order_, [](IAction*) {},
				[](const IAction*) { return true; }, std::function<bool(const IAction*)>(nullptr), "", true);
				auto application = self_.get<IUIApplication>();
				assert(application != nullptr);
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

	std::unique_ptr<IAction> createAction(const char* id, const char* text, const char* path, int actionOrder,
	                                      std::function<void(IAction*)> func,
	                                      std::function<bool(const IAction*)> enableFunc,
	                                      std::function<bool(const IAction*)> checkedFunc)
	{
		std::unique_ptr<IAction> action;

		auto order = actionOrder;
		std::string shortcut;
		bool hasUserShortcut = false;
		auto shortcutIt = currentActionShortcuts_.find(id);
		if (shortcutIt != currentActionShortcuts_.end())
		{
			shortcut = shortcutIt->second;
			hasUserShortcut = true;
		}

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
			currentShortcutCollection_[id] = hasUserShortcut ? shortcut : actionData.shortcut_;

			action =
			self_.createAction(actionData.text_.empty() && !actionData.isSeparator_ ? text : actionData.text_.c_str(),
			                   actionData.icon_.c_str(), actionData.windowId_.c_str(),
			                   actionData.path_.empty() ? path : actionData.path_.c_str(),
			                   hasUserShortcut ? shortcut.c_str() : actionData.shortcut_.c_str(),
			                   actionData.order_ == s_defaultActionOrder ? order : actionData.order_, func, enableFunc,
			                   checkedFunc, actionData.group_.c_str());
		}
		else
		{
			auto& defaultActionShortcuts = shortcutModel_->getDefaultActionShortcuts();
			defaultActionShortcuts[id] = "";
			currentShortcutCollection_[id] = shortcut;

			// Fall back to creating an action with the passed in text and path
			action = self_.createAction(text, "", "", path, shortcut.c_str(), order, func, enableFunc, checkedFunc, "");
		}

		assert(action != nullptr);
		auto pAction = action.get();
		actions_[id] = pAction;
		return action;
	}

	std::unique_ptr<IAction> createAction(const char* id, std::function<void(IAction*)> func,
	                                      std::function<bool(const IAction*)> enableFunc,
	                                      std::function<bool(const IAction*)> checkedFunc)
	{
		auto tok = strrchr(id, ':');
		auto order = tok != nullptr ? strtol(tok + 1, nullptr, 0) : s_defaultActionOrder;
		auto fullPath = tok != nullptr ? std::string(id, tok - id) : id;

		id = fullPath.c_str();
		tok = strrchr(id, '.');
		auto text = tok != nullptr ? tok + 1 : fullPath;
		auto path = tok != nullptr ? std::string(id, tok - id) : "";

		return createAction(id, text.c_str(), path.c_str(), order, func, enableFunc, checkedFunc);
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
		assert(window != nullptr);
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
				assert(action != nullptr);
				if (value != action->shortcut())
				{
					action->setShortcut(value.c_str());
					changed = true;
				}
			}
			if (changed)
			{
				auto framework = self_.get<IUIFramework>();
				assert(framework != nullptr);
				framework->signalKeyBindingsChanged();
			}
		});
	}

	ActionManager& self_;
	std::unordered_map<std::string, std::unique_ptr<ActionData>> defaultActionData_;
	Collection currentShortcutCollection_;
	std::unordered_map<std::string, std::string> currentActionShortcuts_;
	ObjectHandleT<ShortcutDialogModel> shortcutModel_;
	std::unordered_map<std::string, IAction*> actions_;
	std::unordered_map<std::string, std::unique_ptr<IAction>> actionsSeparators_;
	ConnectionHolder connections_;
};

ActionManager::ActionManager(IComponentContext& contextManager) : Depends(contextManager), impl_(new Impl(*this))
{
	auto defManager = get<IDefinitionManager>();
	auto& definitionManager = *defManager;
	REGISTER_DEFINITION(ShortcutDialogModel);
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

std::unique_ptr<IAction> ActionManager::createAction(const char* id, std::function<void(IAction*)> func,
                                                     std::function<bool(const IAction*)> enableFunc,
                                                     std::function<bool(const IAction*)> checkedFunc)
{
	return impl_->createAction(id, func, enableFunc, checkedFunc);
}

std::unique_ptr<IAction> ActionManager::createAction(const char* id, const char* text, const char* path,
                                                     int actionOrder, std::function<void(IAction*)> func,
                                                     std::function<bool(const IAction*)> enableFunc,
                                                     std::function<bool(const IAction*)> checkedFunc)
{
	return impl_->createAction(id, text, path, actionOrder, func, enableFunc, checkedFunc);
}

void ActionManager::loadActionData(IDataStream& dataStream)
{
	TextStream stream(dataStream);
	impl_->loadActionData(stream);
}

ObjectHandle ActionManager::getContextObject() const
{
	return impl_->shortcutModel_;
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
