#ifndef _THREADLOCALVALUE_IPP_
#define _THREADLOCALVALUE_IPP_
#pragma once

#include "core_common/assert.hpp"

namespace wgt
{
template <typename T, bool Small>
ThreadLocalValueImpl<T, Small>::ThreadLocalValueImpl() : m_tlsId(ThreadLocalBase::AllocTlsId())
{
	ThreadLocalValueImpl<T, Small>::Initialize();
}

template <typename T, bool Small>
ThreadLocalValueImpl<T, Small>::~ThreadLocalValueImpl()
{
	ThreadLocalValueImpl<T, Small>::Shutdown();
	ThreadLocalBase::FreeTlsId(m_tlsId);
}

template <typename T, bool Small>
T& ThreadLocalValueImpl<T, Small>::GetValue()
{
	T* t = reinterpret_cast<T*>(ThreadLocalBase::GetTlsValue(m_tlsId));
	TF_ASSERT(t != nullptr);

	return *t;
}

template <typename T, bool Small>
void ThreadLocalValueImpl<T, Small>::Initialize()
{
	T* t = new T(T());
	ThreadLocalBase::SetTlsValue(m_tlsId, t);
}

template <typename T, bool Small>
void ThreadLocalValueImpl<T, Small>::Shutdown()
{
	T* t = reinterpret_cast<T*>(ThreadLocalBase::GetTlsValue(m_tlsId));
	TF_ASSERT(t != nullptr);

	delete t;
}

template <typename T>
ThreadLocalValueImpl<T, true>::ThreadLocalValueImpl() : m_tlsId(ThreadLocalBase::AllocTlsId())
{
	ThreadLocalValueImpl<T, true>::Initialize();
}

template <typename T>
ThreadLocalValueImpl<T, true>::ThreadLocalValueImpl(T t) : m_tlsId(ThreadLocalBase::AllocTlsId())
{
	ThreadLocalValueImpl<T, true>::Initialize();
	SetValue(t);
}

template <typename T>
ThreadLocalValueImpl<T, true>::~ThreadLocalValueImpl()
{
	ThreadLocalValueImpl<T, true>::Shutdown();
	ThreadLocalBase::FreeTlsId(m_tlsId);
}

template <typename T>
T ThreadLocalValueImpl<T, true>::SetValue(T t)
{
	UnionT tempUnion;
	tempUnion.value = t;
	ThreadLocalBase::SetTlsValue(m_tlsId, tempUnion.voidPtr);
	return tempUnion.value;
}

template <typename T>
T ThreadLocalValueImpl<T, true>::GetValue()
{
	UnionT tempUnion;
	tempUnion.voidPtr = ThreadLocalBase::GetTlsValue(m_tlsId);
	return tempUnion.value;
}

template <typename T>
void ThreadLocalValueImpl<T, true>::Initialize()
{
	UnionT tempUnion;
	tempUnion.value = T();
	ThreadLocalBase::SetTlsValue(m_tlsId, tempUnion.voidPtr);
}

template <typename T>
void ThreadLocalValueImpl<T, true>::Shutdown()
{
}
} // end namespace wgt
#endif // _THREADLOCALVALUE_IPP_
