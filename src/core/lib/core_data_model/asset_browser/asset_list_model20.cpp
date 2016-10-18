#include "asset_list_model20.hpp"
#include "base_asset_object_item20.hpp"

namespace wgt
{
namespace AssetBrowser20
{
AbstractItem* AssetListModel::item(int row) const
{
	if (row >= 0 && row < (int)items_.size())
	{
		return items_.at(size_t(row)).get();
	}

	return nullptr;
}

int AssetListModel::index(const AbstractItem* item) const
{
	int result = -1;
	int i = 0;
	bool bFound = false;
	for (auto iter = items_.begin(); iter != items_.end(); iter++)
	{
		if (iter->get() == item)
		{
			bFound = true;
			break;
		}
		i++;
	}
	if (bFound)
	{
		result = i;
	}
	return result;
}

int AssetListModel::rowCount() const
{
	return static_cast<int>(items_.size());
}

int AssetListModel::columnCount() const
{
	return 1;
}

std::vector<std::string> AssetListModel::roles() const
{
	std::vector<std::string> roles;
	roles.push_back(ItemRole::indexPathName);
	roles.push_back(ItemRole::thumbnailName);
	roles.push_back(ItemRole::typeIconName);
	roles.push_back(ItemRole::assetSizeName);
	roles.push_back(ItemRole::createdTimeName);
	roles.push_back(ItemRole::modifiedTimeName);
	roles.push_back(ItemRole::accessedTimeName);
	roles.push_back(ItemRole::isDirectoryName);
	roles.push_back(ItemRole::isReadOnlyName);
	roles.push_back(ItemRole::isCompressedName);
	roles.push_back(ItemRole::statusIconName);
	roles.push_back(ItemRole::itemIdName);

	return roles;
}

void AssetListModel::clear()
{
	int count = rowCount();
	if (count == 0)
	{
		return;
	}
	preRowsRemoved_(0, count);
	items_.clear();
	postRowsRemoved_(0, count);
}

void AssetListModel::push_back(IAssetObjectItem* value)
{
	auto index = static_cast<int>(items_.size());

	preRowsInserted_(index, 1);
	items_.emplace(items_.end(), std::move(value));
	postRowsInserted_(index, 1);
}

void AssetListModel::push_front(IAssetObjectItem* value)
{
	int index = 0;

	preRowsInserted_(index, 1);
	items_.emplace(items_.begin(), std::move(value));
	postRowsInserted_(index, 1);
}

const char* mimeKey = "text/uri-list";

std::vector<std::string> AssetListModel::mimeTypes() const
{
	std::vector<std::string> result(1);
	result.push_back(mimeKey);
	return result;
}

MimeData AssetListModel::mimeData(std::vector<ItemIndex>& indices)
{
	std::string paths;

	for (auto& index : indices)
	{
		AbstractItem* fileItem = this->item(index.row_);
		Variant pathVariant = fileItem->getData(0, 0, ItemRole::indexPathId);

		std::string path = pathVariant.cast<std::string>();
		paths += "file:///";
		paths += path;
		paths += "\r\n";
	}

	MimeData result;

	if (paths.size() > 0)
	{
		result[mimeKey] = std::vector<char>(paths.begin(), paths.end());
	}

	return result;
}

AssetListModel::AssetListModel()
{
}

AssetListModel::AssetListModel(const AssetListModel& rhs)
{
}

AssetListModel::~AssetListModel()
{
}

Connection AssetListModel::connectPreRowsInserted(RangeCallback callback)
{
	return preRowsInserted_.connect(callback);
}

Connection AssetListModel::connectPostRowsInserted(RangeCallback callback)
{
	return postRowsInserted_.connect(callback);
}

Connection AssetListModel::connectPreRowsRemoved(RangeCallback callback)
{
	return preRowsRemoved_.connect(callback);
}

Connection AssetListModel::connectPostRowsRemoved(RangeCallback callback)
{
	return postRowsRemoved_.connect(callback);
}

AssetListModel& AssetListModel::operator=(const AssetListModel& rhs)
{
	return *this;
}

IAssetObjectItem& AssetListModel::operator[](size_t index)
{
	return *items_[index].get();
}

const IAssetObjectItem& AssetListModel::operator[](size_t index) const
{
	return *items_[index].get();
}
} // end namespace AssetBrowser20
} // end namespace wgt
