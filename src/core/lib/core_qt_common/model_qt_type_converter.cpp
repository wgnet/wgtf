#include "model_qt_type_converter.hpp"

#include "core_common/assert.hpp"
#include "core_data_model/abstract_item_model.hpp"
#include "core_qt_common/models/qt_item_model.hpp"
#include "core_reflection/object_handle.hpp"

#include <QQmlEngine>

namespace wgt
{
bool ModelQtTypeConverter::toVariant(const QVariant& qVariant, Variant& o_variant) const
{
	if (auto model = QtItemModel<QtTableModel>::fromQVariant(qVariant))
	{
        typedef std::decay<decltype(model->source())>::type Type;
		o_variant = &model->source();
		return true;
	}

	if (auto model = QtItemModel<QtTreeModel>::fromQVariant(qVariant))
	{
        typedef std::decay<decltype(model->source())>::type Type;
		o_variant = &model->source();
		return true;
	}

	if (auto model = QtItemModel<QtListModel>::fromQVariant(qVariant))
	{
        typedef std::decay<decltype(model->source())>::type Type;
		o_variant = &model->source();
		return true;
	}

	if (auto model = QtItemModel<QtAbstractItemModel>::fromQVariant(qVariant))
	{
        typedef std::decay<decltype(model->source())>::type Type;
		o_variant = &model->source();
		return true;
	}

	return false;
}

template <class Model>
bool convertModel(const Model* source, QVariant& o_qVariant, QObject* parent)
{
	if (source == nullptr)
	{
		return false;
	}

	typedef typename QtItemModelTypeHelper<Model>::WrapperType ModelWrapper;

	TF_ASSERT(parent != nullptr);
	auto model = new ModelWrapper(const_cast<Model&>(*source));
	model->setParent(parent);
	o_qVariant = QVariant::fromValue(model);

	return true;
}

bool ModelQtTypeConverter::toQVariant(const Variant& variant, QVariant& o_qVariant, QObject* parent) const
{
	if (variant.typeIs<AbstractItemModel>() &&
	    convertModel(variant.cast<const AbstractItemModel*>(), o_qVariant, parent))
	{
		return true;
	}

	if (variant.typeIs<AbstractListModel>() &&
	    convertModel(variant.cast<const AbstractListModel*>(), o_qVariant, parent))
	{
		return true;
	}

	if (variant.typeIs<AbstractTreeModel>() &&
	    convertModel(variant.cast<const AbstractTreeModel*>(), o_qVariant, parent))
	{
		return true;
	}

	if (variant.typeIs<AbstractTableModel>() &&
	    convertModel(variant.cast<const AbstractTableModel*>(), o_qVariant, parent))
	{
		return true;
	}

	if (!variant.typeIs<ObjectHandle>())
	{
		return false;
	}

	auto provider = variant.cast<ObjectHandle>();

	if (provider == nullptr)
	{
		return false;
	}

	if (convertModel(provider.getBase<AbstractItemModel>(), o_qVariant, parent))
	{
		return true;
	}

	if (convertModel(provider.getBase<AbstractListModel>(), o_qVariant, parent))
	{
		return true;
	}

	if (convertModel(provider.getBase<AbstractTreeModel>(), o_qVariant, parent))
	{
		return true;
	}

	if (convertModel(provider.getBase<AbstractTableModel>(), o_qVariant, parent))
	{
		return true;
	}

	return false;
}
} // end namespace wgt
