#pragma once

#include <unordered_map>
#include <vector>

#include "static_string_database/i_static_string_database.hpp"

namespace wgt
{

/**
 * Simple IStaticStringDatabase implementation. TODO: rename to blablablaSimple
**/
class StaticStringDatabase: public IStaticStringDatabase
{
public:
	/**
	 * Tests if \id exists in database.
	 *
	 * @param id String id.
	 * @return True if \id is in database, false otherwise.
	**/
	virtual bool hasId(uint64_t id) const override;

	/**
	 * Finds string id.
	 *
	 * @param string String value.
	 * @return String id if \string is in database, \npos() value otherwise.
	**/
	virtual uint64_t find(const char* string) const override;

	/**
	 * Inserts string if not exist.
	 *
	 * @param string String value.
	 * @return Id of string inserted or id of string in database if adding string that is already there.
	**/
	virtual uint64_t insert(const char* string) override;

	/**
	 * Gets id to string map.
	 *
	 * @return A collection of strings indexed by ids.
	**/
	virtual const Collection idToStringMapping() const override;

	/**
	 * Gets Zero terminated string by Id
	 *
	 * @param id String id.
	 * @return Pointer to zero terminated string if id in database, nullptr otherwise.
	**/
	virtual const char* getText(uint64_t id) const override;

private:
	std::vector<std::string> m_strings;
	std::unordered_map<std::string, uint64_t> m_keys;
};

}
