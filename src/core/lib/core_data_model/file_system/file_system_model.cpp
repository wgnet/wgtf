#include "file_system_model.hpp"
#include "core_data_model/abstract_item.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_serialization/i_file_system.hpp"

#include <string>

namespace wgt
{
namespace
{
	class FileItem;
	typedef std::vector< std::unique_ptr< FileItem > > FileItems;

	class FileItem : public AbstractTreeItem
	{
	public:
		FileItem( const IFileInfoPtr& fileInfo, const FileItem * parent )
			: fileInfo_( fileInfo )
			, parent_( parent )
		{

		}

		Variant getData( int column, size_t roleId ) const override
		{
			static size_t displayRole = ItemRole::compute( "display" );
			if (roleId == displayRole)
			{
				return fileInfo_->name();
			}

			if (roleId == IndexPathRole::roleId_)
			{
				return fileInfo_->fullPath();
			}

			return Variant();
		}

		bool setData( int column, size_t roleId, const Variant & data ) override
		{
			throw std::logic_error("The method or operation is not implemented.");
		}

		const IFileInfoPtr fileInfo_;
		const FileItem * parent_;
		const FileItems children_;
	};
}

struct FileSystemModel::Impl 
{
	Impl( IFileSystem & fileSystem, const char * rootDirectory )
		: fileSystem_( fileSystem )
		, rootDirectory_( rootDirectory )
	{
	}

	IFileSystem & fileSystem_;
	std::string rootDirectory_;
	FileItems rootItems_;
};

FileSystemModel::FileSystemModel( IFileSystem & fileSystem, const char * rootDirectory )
	: impl_( new Impl( fileSystem, rootDirectory ) )
{
}

FileSystemModel::~FileSystemModel()
{
}

AbstractItem * FileSystemModel::item( const ItemIndex & index ) const
{
	auto parentItem = static_cast< const FileItem * >( index.parent_ );

	// See if we have cached this item already
	auto & fileItems = parentItem != nullptr ? parentItem->children_ : impl_->rootItems_;
	if (index.row_ < static_cast< int >( fileItems.size() ) )
	{
		auto fileItem = fileItems[index.row_].get();
		if (fileItem != nullptr)
		{
			return fileItem;
		}
	}

	// Item not cached, must enumerate
	const auto directory = parentItem != nullptr ?
		parentItem->fileInfo_->fullPath() :
		impl_->rootDirectory_.c_str();
	int i = 0;
	impl_->fileSystem_.enumerate( directory, [&]( IFileInfoPtr&& fileInfo )
	{
		// Skip dots and hidden files
		if (fileInfo->isDots() || fileInfo->isHidden())
		{
			return true;
		}

		if (i == static_cast< int >( fileItems.size() ))
		{
			std::unique_ptr< FileItem > fileItem( new FileItem( std::move( fileInfo ), parentItem ) );
			const_cast< FileItems & >( fileItems ).emplace_back( std::move( fileItem ) );
			if (i == index.row_)
			{
				return false;
			}
		}
		++i;
		return true;
	} );
	return fileItems.back().get();
}

AbstractTreeModel::ItemIndex FileSystemModel::index( const AbstractItem * item ) const
{
	assert( item != nullptr );
	auto fileItem = static_cast< const FileItem * >( item );
	auto parentItem = fileItem->parent_;

	// See if we have cached this item already
	auto & fileItems = parentItem != nullptr ? parentItem->children_ : impl_->rootItems_;
	auto findIt = std::find_if( fileItems.begin(), fileItems.end(), [&]( const std::unique_ptr< FileItem > & value )
	{
		return value.get() == fileItem;
	} );
	if (findIt != fileItems.end())
	{
		return ItemIndex( static_cast< int >( std::distance( fileItems.begin(), findIt ) ), parentItem );
	}

	// Item not cached, must enumerate
	const auto directory = parentItem != nullptr ?
		parentItem->fileInfo_->fullPath() :
		impl_->rootDirectory_.c_str();
	int i = 0;
	impl_->fileSystem_.enumerate( directory, [&]( IFileInfoPtr && fileInfo )
	{
		// Skip dots and hidden files
		if (fileInfo->isDots() || fileInfo->isHidden())
		{
			return true;
		}

		if (strcmp( fileItem->fileInfo_->name(), fileInfo->name() ) == 0)
		{
			return false;
		}
		++i;
		return true;
	} );
	return ItemIndex( i, parentItem );
}

int FileSystemModel::rowCount( const AbstractItem * item ) const
{
	auto fileItem = static_cast< const FileItem * >( item );

	const auto directory = fileItem != nullptr ?
		fileItem->fileInfo_->fullPath() :
		impl_->rootDirectory_.c_str();
	int count = 0;
	impl_->fileSystem_.enumerate( directory, [&]( IFileInfoPtr && fileInfo )
	{
		// Skip dots and hidden files
		if (fileInfo->isDots() || fileInfo->isHidden())
		{
			return true;
		}

		++count;
		return true;
	} );
	return count;
}

int FileSystemModel::columnCount() const
{
	return 1;
}
} // end namespace wgt
