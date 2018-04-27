#ifndef REFLECTION_CACHE_HPP
#define REFLECTION_CACHE_HPP

#include "property_accessor_data.hpp"
#include <unordered_map>
#include <map>

namespace wgt
{

class IClassDefinition;

namespace ReflectionPrivate
{
	typedef uint64_t PropertyId;

	//Need to use a concrete type instead of typedef because it goes past the maximum decorated 
	//length
	struct PropertyDataMap
		: public std::unordered_map< PropertyId, std::shared_ptr< PropertyAccessorPrivate::Data > >
	{

	};

	typedef std::unordered_map<
		const IClassDefinition *,
		std::unordered_map< Variant, PropertyDataMap > > PropertyCache;

	typedef std::unordered_map<
		std::shared_ptr< PropertyAccessorPrivate::Data >,
		Variant > PropertyAccessorValueCache;

	uint64_t computePropertyId(const char * path);

	struct ReflectionCache
	{
		PropertyCache propCache_;
		PropertyAccessorValueCache valueCache_;
	};

	extern std::unique_ptr< ReflectionCache > s_Cache;

	extern int s_CacheBatchRefs;
}

}

#endif //REFLECTION_CACHE_HPP