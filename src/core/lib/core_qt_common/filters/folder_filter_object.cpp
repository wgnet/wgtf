#include "folder_filter_object.hpp"
#include "core_data_model/abstract_item.hpp"
#include "core_logging/logging.hpp"

namespace wgt
{
ITEMROLE(isDirectory)
ITEMROLE(isFolder)

//------------------------------------------------------------------------------
FolderFilterObject::FolderFilterObject()
	: QtFilterObject( false )
{
	setFilterRole(Directory);
}

//------------------------------------------------------------------------------
FolderFilterObject::~FolderFilterObject()
{
}

//------------------------------------------------------------------------------
bool FolderFilterObject::filterAcceptsRowValid() const 
{
	return true;
}

//------------------------------------------------------------------------------
bool FolderFilterObject::filterAcceptsRow(const Variant& variant) const 
{
	auto item = reinterpret_cast<AbstractItem*>(variant.value<intptr_t>());
	auto treeItem = dynamic_cast< AbstractTreeItem * >( item );
	if (treeItem == nullptr)
	{
		return true;
	}

	const auto isFolder = treeItem->getData(0, filterRoleId_);
	if (!isFolder.canCast<bool>() || isFolder.cast<bool>())
	{
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
void FolderFilterObject::setFilterName(const QString& filterName)
{
	filterName_ = filterName.toUtf8().data();
}

//-----------------------------------------------------------------------------
QString FolderFilterObject::getFilterName() const
{
	return QString(filterName_.c_str());
}

//-----------------------------------------------------------------------------
FolderFilterObject::FolderFilterRole FolderFilterObject::getFilterRole() const
{
	return filterRole_;
}

//-----------------------------------------------------------------------------
void FolderFilterObject::setFilterRole(FolderFilterRole filterRole)
{
	filterRole_ = filterRole;
	switch (filterRole_)
	{
	case FolderFilterObject::Directory:
		filterRoleId_ = ItemRole::isDirectoryId;
		break;
	case FolderFilterObject::Folder:
		filterRoleId_ = ItemRole::isFolderId;
		break;
	default:
		filterRoleId_ = ItemRole::isDirectoryId;
		NGT_ERROR_MSG("FolderFilterObject: Unknown filter role set");
	}
}

}

