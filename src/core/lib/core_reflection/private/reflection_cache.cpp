#include "reflection_cache.hpp"
#include "wg_types/hash_utilities.hpp"

namespace wgt
{

namespace ReflectionPrivate
{
	uint64_t computePropertyId(const char * path)
	{
		return HashUtilities::compute(path);
	}

	std::unique_ptr< ReflectionCache > s_Cache;

	int s_CacheBatchRefs = 0;
}

}

