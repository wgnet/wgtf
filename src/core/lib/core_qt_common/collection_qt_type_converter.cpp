#include "collection_qt_type_converter.hpp"

#include "core_data_model/collection_model_old.hpp"
#include "core_data_model/collection_model.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_variant/collection.hpp"
#include "core_qt_common/models/qt_collection_model.hpp"

namespace wgt
{
CollectionQtTypeConverter::CollectionQtTypeConverter(IComponentContext& context) : context_(context)
{
}

/**
*	Convert a QVariant to a Collection.
*	Note that o_variant will be referencing the collection in qVariant,
*	does not perform a copy.
*	The qVariant needs to be kept alive for the lifetime of o_variant.
*	A CollectionHolder needs to be used to perform a copy.
*/
bool CollectionQtTypeConverter::toVariant(const QVariant& qVariant, Variant& o_variant) const
{
	int typeId = qVariant.type();
	if (typeId == QVariant::UserType)
	{
		typeId = qVariant.userType();
	}

	if (!qVariant.canConvert<QtCollectionModel*>())
	{
		return false;
	}

	auto model = qVariant.value<QtCollectionModel*>();
	o_variant = model->source().getSource();
	return true;
}

/**
*	Convert a Collection to QVariant.
*	Note that o_qVariant will be referencing the collection in variant,
*	does not copy it.
*	The variant needs to be kept alive to the lifetime of o_qVariant.
*	A CollectionHolder needs to be used to perform a copy.
*/
bool CollectionQtTypeConverter::toQVariant(const Variant& variant, QVariant& o_qVariant, QObject* parent) const
{
	if (!variant.typeIs<Collection>())
	{
		return false;
	}

	Collection value;
	if (!variant.tryCast(value))
	{
		return false;
	}

	assert(parent != nullptr);

	// Create a new collection model
	auto collectionModel = new CollectionModel();
	collectionModel->setSource(value);

	auto model = new QtCollectionModel(context_, std::unique_ptr<CollectionModel>(collectionModel));
	model->setParent(parent);
	o_qVariant = QVariant::fromValue(model);

	return true;
}
} // end namespace wgt
