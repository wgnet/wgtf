#include "header_footer_text_extension.hpp"
#include "core_data_model/i_item.hpp"
#include "core_data_model/i_list_model.hpp"
#include "core_qt_common/models/wg_list_model.hpp"
#include "core_qt_common/models/wg_tree_model.hpp"

namespace wgt
{
HeaderFooterTextExtension::HeaderFooterTextExtension()
{
}

HeaderFooterTextExtension::~HeaderFooterTextExtension()
{
}

QHash<int, QByteArray> HeaderFooterTextExtension::roleNames() const
{
	QHash<int, QByteArray> roleNames;
	registerRole(headerTextRole::roleName_, roleNames);
	registerRole(footerTextRole::roleName_, roleNames);
	return roleNames;
}
} // end namespace wgt
