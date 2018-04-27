#pragma once

#include "core_variant/collection.hpp"

#include <cstdint>
#include <limits>

namespace wgt
{

/**
 * Interface that provide access to string database by numeric Id
**/
class IStaticStringDatabase
{
public:
	/**
	 * Defines index that is not in database.
	 *
	 * @return Index used as not found indicator.
	**/
	static uint64_t npos()
	{
		// TODO: compile everything with /DNOMINMAX and use normal form
		return (std::numeric_limits<uint64_t>::max)();
	}

	virtual ~IStaticStringDatabase() {}

	/**
	 * Tests if \id exists in database.
	 *
	 * @param id String id.
	 * @return True if \id is in database, false otherwise.
	**/
	virtual bool hasId(uint64_t id) const = 0;

	/**
	 * Finds string id.
	 *
	 * @param string String value.
	 * @return String id if \string is in database, \npos() value otherwise.
	**/
	virtual uint64_t find(const char* string) const = 0;

	/**
	 * Inserts string if not exist.
	 *
	 * @param string String value.
	 * @return Id of string inserted or id of string in database if adding string that is already there.
	**/
	virtual uint64_t insert(const char* string) = 0;

	/**
	 * Gets id to string map.
	 *
	 * @return A collection of strings indexed by ids.
	**/
	virtual const Collection idToStringMapping() const = 0;

	/**
	 * Gets Zero terminated string by Id
	 *
	 * @param id String id.
	 * @return Pointer to zero terminated string if id in database, nullptr otherwise.
	**/
	virtual const char* getText(uint64_t id) const = 0;
};

}
