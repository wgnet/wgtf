#include "thumbnail_extension.hpp"

#include "core_common/assert.hpp"
#include "core_data_model/i_item.hpp"
#include "core_data_model/i_list_model.hpp"

#include "core_qt_common/i_qt_framework.hpp"
#include "core_qt_common/models/wg_list_model.hpp"
#include "core_qt_common/qt_image_provider_old.hpp"

namespace wgt
{
ThumbnailExtension::~ThumbnailExtension()
{
}

QHash<int, QByteArray> ThumbnailExtension::roleNames() const
{
	QHash<int, QByteArray> roleNames;
	registerRole(ThumbnailRole::roleName_, roleNames);
	return roleNames;
}

QVariant ThumbnailExtension::data(const QModelIndex& index, int role) const
{
	ItemRole::Id roleId;
	if (!decodeRole(role, roleId))
	{
		return QVariant(QVariant::Invalid);
	}

	TF_ASSERT(index.isValid());
	auto item = reinterpret_cast<IItem*>(index.internalPointer());
	if (item == nullptr)
	{
		return false;
	}
	auto column = index.column();

	if (roleId == ThumbnailRole::roleId_)
	{
		auto thumbnail = item->getThumbnail(column);
		if (thumbnail != nullptr)
		{
			auto qtImageProvider = dynamic_cast<QtImageProviderOld*>(
			get<IQtFramework>()->qmlEngine()->imageProvider(QtImageProviderOld::providerId()));
			if (qtImageProvider != nullptr)
			{
				return qtImageProvider->encodeImage(thumbnail);
			}
		}
	}

	return QVariant(QVariant::Invalid);
}

bool ThumbnailExtension::setData(const QModelIndex& index, const QVariant& value, int role)
{
	return false;
}
} // end namespace wgt
