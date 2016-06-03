#ifndef ROLE_PROVIDER_HPP
#define ROLE_PROVIDER_HPP

#include <map>
#include "core_data_model/i_item_role.hpp"

#include <QHash>
#include <QByteArray>

namespace wgt
{
class RoleProvider
{
public:
	virtual ~RoleProvider() { }

	void registerRole( const char * roleName, 
		QHash< int, QByteArray > & o_RoleNames ) const;

	bool encodeRole( size_t roleId, int & o_Role ) const;
	bool decodeRole( int role, size_t & o_RoleId ) const;

private:
	mutable std::map< int, size_t > roleMap_;
};
} // end namespace wgt
#endif
