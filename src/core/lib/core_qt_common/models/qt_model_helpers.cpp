#include "qt_model_helpers.hpp"

#include "core_qt_common/models/extensions/deprecated/i_model_extension_old.hpp"


namespace wgt
{
namespace QtModelHelpers
{
int encodeRole(ItemRole::Id roleId, const Extensions& extensions)
{
	int role = 0;
	for (const auto& extension : extensions)
	{
		if (extension->encodeRole( roleId, role ))
		{
			break;
		}
	}
	return role;
}


int calculateFirst( const size_t start )
{
	return static_cast< int >( start );
}


int calculateLast( const size_t start, const size_t count )
{
	return static_cast< int >( start + count - 1 );
}


} // namespace QtModelHelpers
} // end namespace wgt
