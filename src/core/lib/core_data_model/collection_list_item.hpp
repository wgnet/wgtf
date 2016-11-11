#ifndef COLLECTION_LIST_ITEM_HPP
#define COLLECTION_LIST_ITEM_HPP

#include "core_data_model/i_item.hpp"

#include <memory>

namespace wgt
{
/**
 *	Item to be used by CollectionListModel.
 */
class CollectionListItem : public IItem
{
public:
	CollectionListItem(const char* displayText, const ThumbnailData& thumbnail, const Variant* data, size_t dataLength);

	const char* getDisplayText(int column) const override;
	ThumbnailData getThumbnail(int column) const override;

	Variant getData(int column, size_t roleId) const override;
	bool setData(int column, size_t roleId, const Variant& data) override;

private:
	CollectionListItem(const CollectionListItem& other);
	CollectionListItem(CollectionListItem&& other);
	CollectionListItem& operator=(const CollectionListItem& other);
	CollectionListItem& operator=(CollectionListItem&& other);

	class Impl;
	std::unique_ptr<Impl> impl_;
};
} // end namespace wgt
#endif // COLLECTION_LIST_ITEM_HPP
