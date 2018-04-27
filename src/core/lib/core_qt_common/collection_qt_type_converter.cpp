#include "collection_qt_type_converter.hpp"
#include "i_qt_framework.hpp"
#include "interfaces/i_qt_helpers.hpp"
#include "models/qt_collection_model.hpp"
#include "qt_qlist_memory_fix.hpp"

#include "core_common/assert.hpp"
#include "core_data_model/collection_model.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_variant/collection.hpp"

#include <algorithm>
#include <QQmlEngine>
#include "qt_qlist_memory_fix.hpp"

SPECIALIZE_QLIST(QVariant)

namespace wgt
{
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


	// Try produce a QtCollectionModel
	{
		QtCollectionModel *model = nullptr;
		if (qVariant.canConvert<QJSValue>())
		{
			auto qjsValue = qVariant.value<QJSValue>();
			if (auto *qObject = qjsValue.toQObject())
			{
				model = qobject_cast<QtCollectionModel *>(qObject);
			}
		}
		else if (qVariant.canConvert<QtCollectionModel*>())
		{
			model = qVariant.value<QtCollectionModel *>();
		}

		if (model)
		{
			o_variant = model->source().getSource();
			return true;
		}
	}

	if (qVariant.canConvert<QVariantList>())
	{
		auto pHelpers = get<IQtHelpers>();
		if (pHelpers == nullptr)
		{
			TF_ASSERT(false && "Cannot query IQtHelpers");
			o_variant = Collection();
			return true;
		}
		auto& helpers = (*pHelpers);

		auto qListCopy = qVariant.value<QVariantList>();

		// Need to create a CollectionHolder, otherwise
		// o_variant = Collection(vector);
		// is unsafe, because it takes a reference
		// which will be deleted when children goes out of scope
		typedef std::vector<Variant> ContainerType;
		auto collectionHolder = std::make_shared<CollectionHolder<ContainerType>>();
		ContainerType& vectorCopy = collectionHolder->storage();

		vectorCopy.resize(static_cast<ContainerType::size_type>(qListCopy.size()));
		std::transform(qListCopy.cbegin(),
			qListCopy.cend(),
			vectorCopy.begin(),
			[&helpers](const auto& qVariant) -> Variant
		{
			return helpers.toVariant(qVariant);
		});

		o_variant = Collection(collectionHolder);
		return true;
	}

	return false;
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
	auto qtFramework = get<IQtFramework>();
	if (!variant.typeIs<Collection>() || qtFramework == nullptr || qtFramework->qmlEngine() == nullptr)
	{
		return false;
	}

	Collection value;
	if (!variant.tryCast(value))
	{
		return false;
	}

	// Note that we can't tell if Collection should be converted to a CollectionModel
	// or a QVariantList, so just do CollectionModel by default

	TF_ASSERT(parent != nullptr);

	// Create a new collection model
	auto collectionModel = std::make_unique<CollectionModel>();
	collectionModel->setSource(value);

	auto model = new QtCollectionModel(std::move(collectionModel));
	// Give ownership to the JavaScript Engine which will delete unreferenced objects during garbage collection
	auto qmlEngine = qtFramework->qmlEngine();
	qmlEngine->setObjectOwnership(model, QQmlEngine::ObjectOwnership::JavaScriptOwnership);
	o_qVariant = QVariant::fromValue(qmlEngine->newQObject(model));

	return true;
}
} // end namespace wgt
