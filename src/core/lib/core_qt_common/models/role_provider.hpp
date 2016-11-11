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
	virtual ~RoleProvider()
	{
	}

	enum
	{
		DynamicRole = Qt::UserRole + 256
	};

	void registerRole(const char* roleName, QHash<int, QByteArray>& o_RoleNames) const;

	virtual bool encodeRole(ItemRole::Id roleId, int& o_Role) const;
	virtual bool decodeRole(int role, ItemRole::Id& o_RoleId) const;

private:
	mutable std::map<int, ItemRole::Id> roleMap_;
};
} // end namespace wgt
#endif
