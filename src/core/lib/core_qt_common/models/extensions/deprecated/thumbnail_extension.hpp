#ifndef THUMBNAIL_EXTENSION_HPP
#define THUMBNAIL_EXTENSION_HPP

#include "i_model_extension_old.hpp"

namespace wgt
{
class IQtFramework;

class ThumbnailExtension : public IModelExtensionOld
{
public:
	ThumbnailExtension();
	virtual ~ThumbnailExtension();

	QHash< int, QByteArray > roleNames() const override;
	QVariant data( const QModelIndex &index,
		int role ) const override;
	bool setData( const QModelIndex &index,
		const QVariant &value,
		int role ) override;

private:
	IQtFramework * qtFramework_;
};
} // end namespace wgt
#endif // THUMBNAIL_EXTENSION_HPP
