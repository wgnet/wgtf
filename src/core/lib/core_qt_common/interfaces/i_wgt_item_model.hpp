#ifndef I_WGT_ITEM_MODEL_HPP
#define I_WGT_ITEM_MODEL_HPP

#include <QModelIndex>

namespace wgt
{

class Variant;
class QueryHelper;

class IWgtItemModel
{
public:
	virtual ~IWgtItemModel() {}
	virtual bool canUse( QueryHelper & o_Helper ) const = 0;
	virtual Variant variantData( 
		QueryHelper & helper,
		const QModelIndex& index, int role) const = 0;
};

} //end namespace wgt

#endif //I_WGT_ITEM_MODEL_HPP