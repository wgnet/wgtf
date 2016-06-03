#include "context_object_manager.hpp"

namespace wgt
{
ContextObjectManager::ContextObjectManager( IObjectManagerNew & parent, const wchar_t * contextName )
	: parent_( parent )
{

}
} // end namespace wgt
