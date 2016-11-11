#ifndef _THREADLOCALVALUE_HPP_
#define _THREADLOCALVALUE_HPP_
#pragma once

#include <type_traits>

#ifdef __APPLE__
#include <pthread.h>
#endif

namespace wgt
{
#ifdef __APPLE__
typedef pthread_key_t TLIndexType;
#else
typedef unsigned int TLIndexType;
#endif

class ThreadLocalBase
{
public:
	ThreadLocalBase();

	static void InitializeThreadLocalStorage();
	static void ShutdownThreadLocalStorage();

protected:
	~ThreadLocalBase();

	static TLIndexType AllocTlsId();
	static void FreeTlsId(TLIndexType id);
	static void SetTlsValue(TLIndexType id, void* value);
	static void* GetTlsValue(TLIndexType id);

private:
	virtual void Initialize() = 0;
	virtual void Shutdown() = 0;
};

template <typename T, bool Small>
class ThreadLocalValueImpl : public ThreadLocalBase
{
public:
	ThreadLocalValueImpl();
	~ThreadLocalValueImpl();

	T& GetValue();

private:
	virtual void Initialize();
	virtual void Shutdown();

	TLIndexType m_tlsId;
};

template <typename T>
class ThreadLocalValueImpl<T, true> : public ThreadLocalBase
{
public:
	ThreadLocalValueImpl();
	ThreadLocalValueImpl(T t);
	~ThreadLocalValueImpl();

	T SetValue(T t);
	T GetValue();

private:
	union UnionT {
		T value;
		void* voidPtr;
	};

	virtual void Initialize();
	virtual void Shutdown();

	TLIndexType m_tlsId;
};

template <typename T>
class ThreadLocalValue : public ThreadLocalValueImpl<T, (sizeof(T) <= sizeof(void*)) && std::is_trivial<T>::value>
{
};
} // end namespace wgt

#include "thread_local_value_impl.hpp"

#define THREAD_LOCAL(TYPE) ThreadLocalValueImpl<TYPE, (sizeof(TYPE) <= sizeof(void*)) && std::is_trivial<TYPE>::value>
#define THREAD_LOCAL_SET(VAR, VALUE) VAR.SetValue(VALUE)
#define THREAD_LOCAL_GET(VAR) VAR.GetValue()
#define THREAD_LOCAL_INC(VAR) VAR.SetValue(VAR.GetValue() + 1)
#define THREAD_LOCAL_DEC(VAR) VAR.SetValue(VAR.GetValue() - 1)

#endif // _THREADLOCALVALUE_HPP_
