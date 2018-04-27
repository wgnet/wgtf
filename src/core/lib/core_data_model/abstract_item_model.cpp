#include "abstract_item_model.hpp"
namespace wgt
{
//------------------------------------------------------------------------------
std::vector<std::string> AbstractItemModel::roles() const
{
	return std::vector<std::string>();
}

//------------------------------------------------------------------------------
void AbstractItemModel::iterateMimeTypes(const std::function<void(const char*)>& iterFunc) const
{
}

//------------------------------------------------------------------------------
std::vector<std::string> AbstractItemModel::mimeTypes() const
{
	return std::vector<std::string>();
}

} // end namespace wgt
