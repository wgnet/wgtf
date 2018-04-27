#ifndef WGT_ITEM_MODEL_BASE_HPP
#define WGT_ITEM_MODEL_BASE_HPP

#include "core_qt_common/interfaces/i_wgt_item_model.hpp"
#include "core_dependency_system/i_interface.hpp"

namespace wgt
{

class Variant;

class QueryHelper
{
public:
	QueryHelper();

	void clear();

	void reset();

	void nextModel();

	void pushModel(const IWgtItemModel & model);

	const IWgtItemModel & getModel() const;

private:
	size_t currentIndex_;
	std::vector< const IWgtItemModel * > itemModels_;
};


class WgtItemModelBase
	: public Implements< IWgtItemModel >
{
public:
	virtual QAbstractItemModel * getSourceModel() const= 0;
	virtual QModelIndex getSourceIndex( const QModelIndex & sourceIndex) const = 0;

	virtual bool canUse(QueryHelper & o_Helper) const override;
	virtual Variant variantData(
		QueryHelper & helper,
		const QModelIndex& index, int role) const override;
};

} //end namespace wgt

#endif //WGT_ITEM_MODEL_BASE_HPP