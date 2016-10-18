#ifndef ASSET_LIST_MODEL20_HPP
#define ASSET_LIST_MODEL20_HPP

#include <memory>
#include "../abstract_item_model.hpp"

namespace wgt
{
namespace AssetBrowser20
{
class IAssetObjectItem;

class AssetListModel : public AbstractListModel
{
public:
	// Typedefs
	typedef std::vector<std::unique_ptr<IAssetObjectItem>> Items;

	virtual AbstractItem* item(int row) const override;

	virtual int index(const AbstractItem* item) const override;

	virtual int rowCount() const override;

	virtual int columnCount() const override;
	virtual std::vector<std::string> roles() const override;

	virtual std::vector<std::string> mimeTypes() const override;
	virtual MimeData mimeData(std::vector<ItemIndex>& indices) override;

	virtual Connection connectPreRowsInserted(RangeCallback callback) override;
	virtual Connection connectPostRowsInserted(RangeCallback callback) override;
	virtual Connection connectPreRowsRemoved(RangeCallback callback) override;
	virtual Connection connectPostRowsRemoved(RangeCallback callback) override;

public:
	AssetListModel();
	virtual ~AssetListModel();

	// List manipulation and usage functions
	void clear();

	IAssetObjectItem& operator[](size_t index);
	const IAssetObjectItem& operator[](size_t index) const;
	void push_back(IAssetObjectItem* value);
	void push_front(IAssetObjectItem* value);

private:
	AssetListModel(const AssetListModel& rhs);
	AssetListModel& operator=(const AssetListModel& rhs);

	Items items_;
	Signal<AbstractListModel::RangeSignature> preRowsInserted_;
	Signal<AbstractListModel::RangeSignature> postRowsInserted_;
	Signal<AbstractListModel::RangeSignature> preRowsRemoved_;
	Signal<AbstractListModel::RangeSignature> postRowsRemoved_;
};
} // end namespace AssetBrowser20

} // end namespace wgt
#endif
