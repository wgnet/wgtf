#ifndef I_ITEM_HPP
#define I_ITEM_HPP

#include "core_variant/variant.hpp"
#include "wg_types/binary_block.hpp"

namespace wgt
{
typedef std::shared_ptr< BinaryBlock > ThumbnailData;

/**
 *	Concept of an item within one of our data models.
 *	Items have data which can be get/set.
 *	Items have "columns" which are just indexes into which piece of
 *	data on the item. (Different to a Qt column).
 */
class IItem
{
public:
	virtual ~IItem() {}

	/// TODO display text will become "data"?
	virtual const char * getDisplayText( int column ) const = 0;

	/// TODO thumbnails will become "data"
	virtual ThumbnailData getThumbnail( int column ) const = 0;
	virtual Variant getData( int column, size_t roleId ) const = 0;
	virtual bool setData( int column, size_t roleId, const Variant & data ) = 0;
};
} // end namespace wgt
#endif // I_ITEM_HPP
