#include "reflection_batch_query.hpp"
#include "core_reflection/private/reflection_cache.hpp"

namespace wgt
{

//------------------------------------------------------------------------------
ReflectionBatchQuery::ReflectionBatchQuery()
{
	if (ReflectionPrivate::s_CacheBatchRefs++ == 0)
	{
		ReflectionPrivate::s_Cache.reset(new ReflectionPrivate::ReflectionCache());
	}
}


//------------------------------------------------------------------------------
ReflectionBatchQuery::~ReflectionBatchQuery()
{
	if (--ReflectionPrivate::s_CacheBatchRefs == 0)
	{
		ReflectionPrivate::s_Cache = nullptr;
	}
}


}