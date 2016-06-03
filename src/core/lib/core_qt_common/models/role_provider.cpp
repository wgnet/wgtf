#include "role_provider.hpp"
#include "core_logging/logging.hpp"

namespace wgt
{
void RoleProvider::registerRole( const char * roleName, 
	QHash< int, QByteArray > & o_RoleNames ) const
{
	auto roleId = ItemRole::compute( roleName );

	// TODO: Need to move this out of roleNames() and into a modelReset callback
	int role = ( roleId % ( INT_MAX - Qt::UserRole ) ) + Qt::UserRole;

	auto it = o_RoleNames.find( role );
	if (it != o_RoleNames.end())
	{
		if (it.value() != roleName)
		{
			NGT_ERROR_MSG( "Cannot not register role %s. Collision detected with role %s\n", 
				roleName, it.value().data() );
		}
		return;
	}

	for (auto roleIt = o_RoleNames.begin(); 
		roleIt != o_RoleNames.end(); ++roleIt)
	{
		if (roleIt.value() == roleName)
		{
			if (roleIt.key() != role)
			{
				NGT_ERROR_MSG( "Cannot register role %s with more than one key.\n", 
					roleName );
				return;
			}
			roleMap_[ role ] = roleId;
			return;
		}
	}

	o_RoleNames[ role ] = roleName;
	roleMap_[ role ] = roleId;
}

bool RoleProvider::encodeRole( size_t roleId, int & o_Role ) const
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

bool RoleProvider::decodeRole( int role, size_t & o_RoleId ) const
{
	auto roleIt = roleMap_.find( role );
	if (roleIt == roleMap_.end())
	{
		return false;
	}

	o_RoleId = roleIt->second;
	return true;
}
} // end namespace wgt
