#ifndef FILE_SYSTEM_MODEL_HPP
#define FILE_SYSTEM_MODEL_HPP

#include "core_data_model/abstract_item_model.hpp"

#include <memory>

namespace wgt
{
class IFileSystem;

class FileSystemModel : public AbstractTreeModel
{
public:
	FileSystemModel(IFileSystem& fileSystem, const char* rootDirectory);
	~FileSystemModel();

	AbstractItem* item(const ItemIndex& index) const override;
	ItemIndex index(const AbstractItem* item) const override;

	int rowCount(const AbstractItem* item) const override;
	int columnCount() const override;

	void iterateRoles(const std::function<void(const char*)>& iterFunc) const override;
	virtual std::vector<std::string> roles() const override;

	virtual MimeData mimeData(std::vector<AbstractItemModel::ItemIndex>& indices) override;
	virtual void iterateMimeTypes(const std::function<void(const char*)>& iterFunc) const override;
	virtual std::vector<std::string> mimeTypes() const override;

	virtual void revert() override;

	virtual Connection connectPreItemDataChanged(DataCallback callback) override;
	virtual Connection connectPostItemDataChanged(DataCallback callback) override;
	virtual Connection connectPreModelReset(VoidCallback callback) override;
	virtual Connection connectPostModelReset(VoidCallback callback) override;

	static const char* FileSystemModel::s_mimeFilePath;
	static const char  FileSystemModel::s_mimeFilePathDelimiter;
private:
	struct Impl;
	std::unique_ptr<Impl> impl_;
};
} // end namespace wgt
#endif // FILE_SYSTEM_MODEL_HPP
