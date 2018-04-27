#ifndef IMAGE_EXTENSION_HPP
#define IMAGE_EXTENSION_HPP

#include "qt_model_extension.hpp"

namespace wgt
{
class ImageExtension : public QtModelExtension
{
public:
	ImageExtension();
	virtual ~ImageExtension();

	QVariant data(const QModelIndex& index, ItemRole::Id roleId) const override;
};
} // end namespace wgt
#endif // IMAGE_EXTENSION_HPP
