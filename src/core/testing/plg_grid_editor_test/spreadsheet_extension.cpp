#include "spreadsheet_extension.hpp"

namespace wgt
{
ITEMROLE(dirty)
ITEMROLE(lastEdit)

SpreadsheetExtension::SpreadsheetExtension()
	: commitTime_(time(nullptr))
{
	roles_.push_back(ItemRole::dirtyName);
	roles_.push_back(ItemRole::lastEditName);
}

SpreadsheetExtension::~SpreadsheetExtension()
{
}

void SpreadsheetExtension::onDataChanged(const ModelIndex& topLeft, const ModelIndex& bottomRight,
                                         const std::vector<ItemRole::Id>& roles)
{
	for(auto& role : roles)
	{
		if(role == ItemRole::dirtyId)
		{
			return;
		}
	}

	if (topLeft != bottomRight)
	{
		// TODO: iterate between topLeft & bottomRight
		return;
	}

	time_t lastEdit = time(nullptr);
	auto it = lastEdits_.find(topLeft);

	if (it != lastEdits_.end())
	{
		lastEdit = it->second;
	}

	std::vector<ItemRole::Id> extRoles;
	extRoles.push_back(ItemRole::lastEditId);

	if (lastEdit != 0 || lastEdit < commitTime_)
	{
		extRoles.push_back(ItemRole::dirtyId);
	}

	lastEdits_[topLeft] = time(nullptr);
	extensionData_->dataChanged(topLeft, bottomRight, extRoles);
}

Variant SpreadsheetExtension::data(const ModelIndex& index, ItemRole::Id roleId) const
{
	if (roleId == ItemRole::dirtyId)
	{
		auto it = lastEdits_.find(index);
		if (it != lastEdits_.end())
		{
			return it->second > commitTime_;
		}
		return false;
	}
	else if (roleId == ItemRole::lastEditId)
	{
		auto it = lastEdits_.find(index);
		if (it != lastEdits_.end())
		{
			return it->second;
		}
		return time(nullptr);
	}

	return Variant();
}

void SpreadsheetExtension::commitData()
{
	std::vector<ItemRole::Id> extRoles;
	extRoles.push_back(ItemRole::dirtyId);

	auto prevCommitTime = commitTime_;
	commitTime_ = time(nullptr);

	for (auto it = lastEdits_.begin(); it != lastEdits_.end(); ++it)
	{
		if (it->second > prevCommitTime)
		{
			ModelIndex index = it->first;

			if (index.isValid())
			{
				extensionData_->dataChanged(index, index, extRoles);
			}
		}
	}
}
} // end namespace wgt
