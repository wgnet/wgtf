#include "thread_local_value.hpp"
#include <vector>
#include <algorithm>

#if defined(_WIN32)
#include <windows.h>
#endif

namespace wgt
{
const int kMaxThreadLocalBases = 2048;

namespace
{
	unsigned int g_numThreadLocalBases = 0;
	typedef ThreadLocalBase* ThreadLocalBasePtr;
	ThreadLocalBasePtr g_threadLocalBases[kMaxThreadLocalBases];
}

typedef std::vector<ThreadLocalBase*> ThreadLocalVector;
ThreadLocalVector& GetThreadLocalVector()
{
	static ThreadLocalVector threadLocalVector;
	return threadLocalVector;
}

ThreadLocalBase::ThreadLocalBase()
{
	g_threadLocalBases[g_numThreadLocalBases++] = this;
}

void ThreadLocalBase::InitializeThreadLocalStorage()
{
	const ThreadLocalBasePtr* begin = g_threadLocalBases;
	const ThreadLocalBasePtr* end = g_threadLocalBases + g_numThreadLocalBases;
	for (auto tlb = begin; tlb != end; ++tlb)
		(*tlb)->Initialize();
}

void ThreadLocalBase::ShutdownThreadLocalStorage()
{
	const ThreadLocalBasePtr* begin = g_threadLocalBases;
	const ThreadLocalBasePtr* end = g_threadLocalBases + g_numThreadLocalBases;
	for (auto tlb = begin; tlb != end; ++tlb)
		(*tlb)->Shutdown();
}

ThreadLocalBase::~ThreadLocalBase()
{
	ThreadLocalBasePtr* begin = g_threadLocalBases;
	ThreadLocalBasePtr* end = g_threadLocalBases + g_numThreadLocalBases;
	ThreadLocalBasePtr* newEnd = std::remove(begin, end, this);
	if (newEnd != end)
	{
		--g_numThreadLocalBases;
		assert(newEnd == end-1);
	}
}

TLIndexType ThreadLocalBase::AllocTlsId()
{
#if defined(__APPLE__)
	pthread_key_t key;
	pthread_key_create(&key, NULL);
	return key;
#else
	return TlsAlloc();
#endif
}

void ThreadLocalBase::FreeTlsId(TLIndexType id)
{
#if defined(__APPLE__)
	pthread_key_delete(id);
#else
	TlsFree(id);
#endif
}

void ThreadLocalBase::SetTlsValue(TLIndexType id, void* value)
{
#if defined(__APPLE__)
	pthread_setspecific(id, value);
#else
	TlsSetValue(id, value);
#endif
}

void* ThreadLocalBase::GetTlsValue(TLIndexType id)
{
#if defined(__APPLE__)
	return pthread_getspecific(id);
#else
	return TlsGetValue(id);
#endif
}
} // end namespace wgt
