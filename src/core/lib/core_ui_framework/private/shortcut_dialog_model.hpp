#ifndef SHORTCUT_DIALOG_MODEL_HPP
#define SHORTCUT_DIALOG_MODEL_HPP

#include "core_data_model/collection_model.hpp"

namespace wgt
{
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
				Collection col = model_.getSource();
				for (size_t i = 0; i < count; i++, iter++)
				{
					isOk = iter.key().tryCast(key);
					assert(isOk);
					isOk = iter.value().tryCast(value);
					assert(isOk);
					col.insertValue(key, value);
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
				Collection col = model_.getSource();
				col[key] = value;
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
			Collection collection(shortcuts_);
			model_.setSource(collection);
			dirty_ = true;
		}

		void onDialogClosed(std::unordered_map<std::string, std::string>& currentActionShortcut,
			std::function<void(bool updateNeeded)> callback = [](bool updateNeeded) {})
		{
			if (!applyChanges_)
			{
				shortcuts_ = currentActionShortcut;
				Collection collection(shortcuts_);
				model_.setSource(collection);
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
} //end namespace wgt

#endif // SHORTCUT_DIALOG_MODEL_HPP