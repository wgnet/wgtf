#include "i_item_role.hpp"

namespace wgt
{
#define X(ROLE, ROLENAME)                          \
	const char* ROLE##Role::roleName_ = #ROLENAME; \
	ItemRole::Id ROLE##Role::roleId_ = ItemRole::compute(#ROLENAME);
ITEM_ROLES
#undef X
} // end namespace wgt
