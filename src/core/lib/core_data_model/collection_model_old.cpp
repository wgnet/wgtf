#include "collection_model_old.hpp"
#include "core_data_model/i_item.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_variant/collection.hpp"
#include "core_serialization/resizing_memory_stream.hpp"


namespace wgt
{
namespace
{
	class CollectionItemOld : public IItem
	{
	public:
		CollectionItemOld(CollectionModelOld & model, size_t index)
			: model_( model )
			, index_( index )
		{

		}

		const char * getDisplayText( int column ) const override
		{
			return nullptr;
		}

		ThumbnailData getThumbnail( int column ) const override
		{
			return nullptr;
		}

		Variant getData( int column, ItemRole::Id roleId ) const override
		{
			auto & collection = model_.getSource();
			if (roleId == ValueTypeRole::roleId_)
			{
				return collection.valueType().getName();
			}
			else if (roleId == KeyTypeRole::roleId_)
			{
				return collection.keyType().getName();
			}
			
			auto it = collection.begin();
			for (size_t i = 0; i < index_ && it != collection.end(); ++i, ++it) {}
			if (it == collection.end())
			{
				return Variant();
			}

			if (roleId == IndexPathRole::roleId_)
			{
				ResizingMemoryStream dataStream;
				TextStream s(dataStream);
				Variant value = it.value();
				s << value;
				return dataStream.takeBuffer();
			}

			if (roleId == ValueRole::roleId_)
			{
				return it.value();
			}
			else if (roleId == KeyRole::roleId_)
			{
				return it.key();
			}

			return Variant();
		}

		bool setData( int column, ItemRole::Id roleId, const Variant & data ) override
		{
			if (roleId != ValueRole::roleId_)
			{
				return false;
			}

			auto & collection = model_.getSource();
			auto it = collection.begin();
			for (size_t i = 0; i < index_ && it != collection.end(); ++i, ++it) {}
			if (it == collection.end())
			{
				return false;
			}

			it.setValue( data );
			return true;
		}

	private:
		CollectionModelOld & model_;
		size_t index_;
	};
}


CollectionModelOld::CollectionModelOld()
{
}


CollectionModelOld::~CollectionModelOld()
{
}


void CollectionModelOld::setSource(Collection & collection)
{
	// TODO emit signal
	items_.clear();
	collection_ = collection;
}


Collection & CollectionModelOld::getSource()
{
	return collection_;
}


IItem * CollectionModelOld::item(size_t index) const
{
	if (items_.size() <= index)
	{
		items_.resize( index + 1 );
	}

	auto item = items_[index].get();
	if (item != nullptr)
	{
		return item;
	}

	item = new CollectionItemOld( *const_cast< CollectionModelOld * >( this ), index );
	items_[index] = std::unique_ptr< IItem >( item );
	return item;
}


size_t CollectionModelOld::index(const IItem* item) const
{
	auto index = 0;
	auto it = items_.begin();
	for (; it != items_.end() && it->get() != item; ++index, ++it) {}
	assert( it != items_.end() );
	return index;
}


bool CollectionModelOld::empty() const
{
	return collection_.empty();
}


size_t CollectionModelOld::size() const
{
	return collection_.size();
}


int CollectionModelOld::columnCount() const
{
	return 1;
}
} // end namespace wgt
