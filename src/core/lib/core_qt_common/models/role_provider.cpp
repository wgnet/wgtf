#include "role_provider.hpp"
#include "core_logging/logging.hpp"

#include <limits>
#include <type_traits>

namespace wgt
{

ITEMROLE(decoration)
ITEMROLE(display)

/* static */ int RoleProvider::convertRole(const char* roleName)
{
	if (strcmp(roleName, ItemRole::displayName) == 0)
	{
		return Qt::DisplayRole;
	}
	else if (strcmp(roleName, ItemRole::decorationName) == 0)
	{
		return Qt::DecorationRole;
	}

	const auto roleId = ItemRole::compute(roleName);
	return RoleProvider::convertRole(roleId);
}

/* static */ int RoleProvider::convertRole(ItemRole::Id roleId)
{
	const auto roleMax = std::numeric_limits<int>::max();
	const int role = static_cast<int>((roleId % (roleMax - DynamicRole)) + DynamicRole);
	return role;
}

//------------------------------------------------------------------------------
bool RoleProvider::registerRole(const char* roleName, QHash<int, QByteArray>& o_RoleNames) const
{

	// TODO: Need to move this out of roleNames() and into a modelReset callback
	const auto roleId = ItemRole::compute(roleName);
	const int role = RoleProvider::convertRole(roleId);

	auto it = o_RoleNames.find(role);
	if (it != o_RoleNames.end())
	{
		if (it.value() != roleName)
		{
			NGT_ERROR_MSG("Cannot not register role %s. Collision detected with role %s\n", roleName,
			              it.value().data());
		}
		return false;
	}

	for (auto roleIt = o_RoleNames.begin(); roleIt != o_RoleNames.end(); ++roleIt)
	{
		if (roleIt.value() == roleName)
		{
			if (roleIt.key() != role && roleIt.key() >= DynamicRole)
			{
				NGT_ERROR_MSG("Cannot register role %s with more than one key, collision with role %d.\n", roleName,
				              roleIt.key());
				return false;
			}
			roleMap_[role] = roleId;
			return true;
		}
	}

	o_RoleNames[role] = roleName;
	roleMap_[role] = roleId;
	return true;
}

bool RoleProvider::encodeRole(ItemRole::Id roleId, int& o_Role) const
{
	for (auto it = roleMap_.begin(); it != roleMap_.end(); ++it)
	{
		if (roleId == it->second)
		{
			o_Role = it->first;
			return true;
		}
	}

	return false;
}

bool RoleProvider::decodeRole(int role, ItemRole::Id& o_RoleId) const
{
	auto roleIt = roleMap_.find(role);
	if (roleIt == roleMap_.end())
	{
		return false;
	}

	o_RoleId = roleIt->second;
	return true;
}
} // end namespace wgt
