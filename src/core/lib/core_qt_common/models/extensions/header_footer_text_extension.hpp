#ifndef HEADER_FOOTER_TEXT_EXTENSION_HPP
#define HEADER_FOOTER_TEXT_EXTENSION_HPP

#include "i_model_extension.hpp"

namespace wgt
{
class HeaderFooterTextExtension: public IModelExtension
{
public:
	HeaderFooterTextExtension();
	virtual ~HeaderFooterTextExtension();

	QHash<int, QByteArray> roleNames() const override;
};
} // end namespace wgt
#endif
