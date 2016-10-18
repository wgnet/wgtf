#ifndef QT_MODEL_HELPERS_HPP
#define QT_MODEL_HELPERS_HPP


#include <memory>
#include <vector>

#include "core_data_model/i_item_role.hpp"

namespace wgt
{
class IModelExtensionOld;

namespace QtModelHelpers
{

typedef std::vector< IModelExtensionOld * > Extensions;

int encodeRole(ItemRole::Id roleId, const Extensions& extensions);
int calculateFirst( const size_t start );
int calculateLast( const size_t start, const size_t count );

} // namespace QtModelHelpers
} // end namespace wgt
#endif // QT_MODEL_HELPERS_HPP
