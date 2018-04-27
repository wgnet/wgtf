#ifndef REFLECTION_METHOD_UTILITIES_HPP
#define REFLECTION_METHOD_UTILITIES_HPP

#include "core_common/assert.hpp"
#include "core_reflection/reflected_method.hpp"
#include "core_reflection/reflected_method_parameters.hpp"
#include "core_variant/type_id.hpp"

#include <functional>

/*
The utilities consist of ReflectedMethodSpecialisation template classes, MethodReturnSplitter and the
ReflectedMethodFactory. The ReflectedMethodSpecialisation implements the ReflectedMethod and handles different
return types and different combinations and number of parameters. The return value is handled by the
MethodReturnSplitter and returns a empty Variant in the case of a void method. ReflectedMethodFactory instantiated the
different ReflectedMethodSpecialisation classes by deducing types from a method as parameter.
*/

namespace wgt
{
template <class Type, bool Reference = false>
struct ReflectedMethodParameterWrapper
{
	ReflectedMethodParameterWrapper(const Variant& variant, const IDefinitionManager& definitionManager)
	{
		ObjectHandle handle;
		value = variant.tryCast<ObjectHandle>(handle) ?
		*reflectedCast<Type>(handle.data(), handle.type(), definitionManager) :
		variant.cast<Type>();
	}

	Type& operator()()
	{
		return value;
	}
	Type value;
};

template <class Type>
struct ReflectedMethodParameterWrapper<Type, true>
{
	ReflectedMethodParameterWrapper(const Variant& variant, const IDefinitionManager& definitionManager)
	{
		if (variant.canCast<ObjectHandle>())
		{
			ObjectHandle handle = variant.cast<ObjectHandle>();
			pointer_ = reflectedCast<Type>(handle.data(), handle.type(), definitionManager);
		}
		else
		{
			pointer_ = &const_cast<Variant&>(variant).cast<Type&>();
		}
	}

	Type& operator()()
	{
		return *pointer_;
	}

	Type* pointer_;
};

template <>
struct ReflectedMethodParameterWrapper<ObjectHandle, false>
{
	ReflectedMethodParameterWrapper(const Variant& variant, const IDefinitionManager&)
	{
		ObjectHandle handle = variant.cast<ObjectHandle>();
		pointer = handle;
	}

	ObjectHandle& operator()()
	{
		return pointer;
	}
	ObjectHandle pointer;
};

template <>
struct ReflectedMethodParameterWrapper<ObjectHandle, true>
{
	ReflectedMethodParameterWrapper(const Variant& variant, const IDefinitionManager&)
	{
		ObjectHandle handle = variant.cast<ObjectHandle>();
		pointer = handle;
	}

	ObjectHandle& operator()()
	{
		return pointer;
	}
	ObjectHandle pointer;
};

template <>
struct ReflectedMethodParameterWrapper<Variant, false>
{
	ReflectedMethodParameterWrapper(const Variant& variant, const IDefinitionManager&) : variant(variant)
	{
	}

	Variant& operator()()
	{
		return variant;
	}
	Variant variant;
};

template <>
struct ReflectedMethodParameterWrapper<Variant, true>
{
	ReflectedMethodParameterWrapper(const Variant& variant, const IDefinitionManager&) : variant(variant)
	{
	}

	const Variant& operator()()
	{
		return variant;
	}
	const Variant& variant;
};

template<class _Ty>
using ParamWrapper = ReflectedMethodParameterWrapper<typename std::decay<_Ty>::type, std::is_reference<_Ty>::value>;

template<class... Args>
struct MethodReturnSplitter
{
protected:
	template <class ClassType, class ReturnType, std::size_t... Is>
	typename std::enable_if<std::is_void<ReturnType>::value, Variant>::type
		invokeMember(ClassType* instance, ReturnType (ClassType::* member)(Args...)
			, const IDefinitionManager& definitionManager
			, const ReflectedMethodParameters& params, std::index_sequence<Is...>)
	{
		auto packedParameters = std::make_tuple(ParamWrapper<Args>(params[Is], definitionManager)...);
		(instance->*member)( std::get<Is>(packedParameters)()... );
		return Variant();
	}

	template <class ClassType, class ReturnType, std::size_t... Is>
	typename std::enable_if<!std::is_void<ReturnType>::value, Variant>::type
		invokeMember(ClassType* instance, ReturnType (ClassType::* member)(Args...)
			, const IDefinitionManager& definitionManager
			, const ReflectedMethodParameters& params, std::index_sequence<Is...>)
	{
		auto packedParameters = std::make_tuple(ParamWrapper<Args>(params[Is], definitionManager)...);
		return (instance->*member)( std::get<Is>(packedParameters)()... );
	}
};


template<class ClassType, class ReturnType, class... Args>
struct ReflectedMethodSpecialisation
	: public MethodReturnSplitter<Args...>
	, public ReflectedMethod
{
	typedef ReturnType ( ClassType::*MethodType )( Args... );
	typedef void (ClassType::*MethodUndoRedoType)(Variant, Variant);

	ReflectedMethodSpecialisation(const char* name, MethodType method, MethodUndoRedoType undoMethod,
	                              MethodUndoRedoType redoMethod)
		: ReflectedMethod(name), method_(method)
	{
		undoMethod_.reset(undoMethod ?
		                  new ReflectedMethodSpecialisation<ClassType, void, Variant, Variant>(
		                  name, undoMethod, nullptr, nullptr) :
		                  nullptr);
		redoMethod_.reset(redoMethod ?
		                  new ReflectedMethodSpecialisation<ClassType, void, Variant, Variant>(
		                  name, redoMethod, nullptr, nullptr) :
		                  nullptr);
	}

	ReflectedMethodSpecialisation( const char* name, MethodType method )
		: ReflectedMethod( name )
		, method_( method )
	{}

	virtual bool isByReference() const
	{
		return std::is_reference<ReturnType>::value;
	}

	Variant invoke(const ObjectHandle& object, const IDefinitionManager& definitionManager,
		const ReflectedMethodParameters& parameters) override
	{
		auto pointer = reflectedCast<ClassType>(object.data(), object.type(), definitionManager);
		TF_ASSERT(pointer != nullptr);
		return invokeMember( pointer, method_, definitionManager, parameters, std::index_sequence_for<Args...>{} );
	}

	size_t parameterCount() const override
	{
		return sizeof...(Args);
	}
	ReflectedMethod* getUndoMethod() override
	{
		return undoMethod_.get();
	}
	ReflectedMethod* getRedoMethod() override
	{
		return redoMethod_.get();
	}

	MethodType method_;
	std::unique_ptr<ReflectedMethod> undoMethod_;
	std::unique_ptr<ReflectedMethod> redoMethod_;
};

struct ReflectedMethodFactory
{
	template <class ClassType, class ReturnType, class... Args>
		static ReflectedMethod* create(
			const char* name, ReturnType (ClassType::*method)(Args...),
			typename ReflectedMethodSpecialisation<ClassType, ReturnType, Args...>::MethodUndoRedoType undoMethod,
			typename ReflectedMethodSpecialisation<ClassType, ReturnType, Args...>::MethodUndoRedoType redoMethod)
	{
		return new ReflectedMethodSpecialisation<ClassType, ReturnType, Args...>(name, method, undoMethod, redoMethod);
	}
};
} // end namespace wgt
#endif // REFLECTION_METHOD_UTILITIES_HPP
