#ifndef REFLECTION_METHOD_UTILITIES_HPP
#define REFLECTION_METHOD_UTILITIES_HPP

#include "core_variant/type_id.hpp"
#include "core_reflection/reflected_method.hpp"
#include "core_reflection/reflected_method_parameters.hpp"

#include <functional>

/*
The utilities consist of ReflectedMethodSpecialisation template classes, MethodReturnSplitter and the
ReflectedMethodFactory. The ReflectedMethodSpecialisation implements the ReflectedMethod and handles different
return types and different combinations and number of parameters. The return value is handled by the
MethodReturnSplitter and returns a empty Variant in the case of a void method. ReflectedMethodFactory instantiated the
different ReflectedMethodSpecialisation classes by deducing types from a method as parameter.
ReflectedMethodSpecialisation and ReflectedMethodFactory entries are created by the RM_METHOD_SPECIALISATION_ and
RM_CREATE_FACTORY_METHOD macros.


Example of a generated ReflectedMethodSpecialisation:

template<class ClassType, class ReturnType, class Parameter0Type, class Parameter1Type>
struct ReflectedMethodSpecialisation<ClassType, ReturnType, Parameter0Type, Parameter1Type>
	: public MethodReturnSplitter<ReturnType>
	, public ReflectedMethod
{
	typedef ReturnType ( ClassType::*MethodType )( Parameter0Type, Parameter1Type );

	ReflectedMethodSpecialisation( const char* name, MethodType method )
		: ReflectedMethod( name )
		, method( method )
	{}

	Variant invoke( const ObjectHandle& object, const ReflectedMethodParameters& parameters ) override
	{
		ClassType* pointer = object.getBase<ClassType>();
		assert( pointer!= nullptr );
		ClassType& target = *pointer;
		ReflectedMethodParameterWrapper<std::decay<Parameter0Type>::type,
			std::is_reference<Parameter0Type>::value> p0 = parameters[0];
		ReflectedMethodParameterWrapper<std::decay<Parameter1Type>::type,
			std::is_reference<Parameter1Type>::value> p1 = parameters[1];
		methodWithoutParameters = std::bind( method, std::ref( target ), std::ref( p0() ), std::ref( p1() ) );
		return MethodReturnSplitter::invoke();
	}

	size_t parameterCount() const override { return 2; }

	MethodType method;
};


Example of a generated factory method:

template<class ClassType, class ReturnType,
	class Parameter1Type, class Parameter2Type>
static ReflectedMethod* create( const char* name, ReturnType ( ClassType::*method )(
	Parameter1Type, Parameter2Type ) )
{
	return new ReflectedMethodSpecialisation<ClassType, ReturnType,
		Parameter1Type, Parameter2Type>( name, method );
}
*/


namespace wgt
{
const size_t MAX_REFLECTED_METHOD_PARAMETER_COUNT = 10;


template<class ReturnType>
struct MethodReturnSplitter
{
protected:
	Variant invoke() { return methodWithoutParameters(); }
	std::function<ReturnType( void )> methodWithoutParameters;
};


template<>
struct MethodReturnSplitter<void>
{
	Variant invoke() { methodWithoutParameters(); return Variant(); }
	std::function<void( void )> methodWithoutParameters;
};


template<class Type, bool Reference = false>
struct ReflectedMethodParameterWrapper
{
	ReflectedMethodParameterWrapper( const Variant& variant, const IDefinitionManager & definitionManager )
	{
		ObjectHandle handle;
		value = variant.tryCast<ObjectHandle>( handle )
			? *reflectedCast< Type >(handle.data(), handle.type(), definitionManager )
			: variant.cast<Type>();
	}

	Type& operator()() { return value; }
	Type value;
};


template<class Type>
struct ReflectedMethodParameterWrapper<Type, true>
{
	ReflectedMethodParameterWrapper( const Variant& variant, const IDefinitionManager & definitionManager )
	{
		ObjectHandle handle = variant.cast<ObjectHandle>();
		pointer = reflectedCast< Type >( handle.data(), handle.type(), definitionManager );
	}

	Type& operator()() { return *pointer; }
	Type* pointer;
};


template<>
struct ReflectedMethodParameterWrapper<ObjectHandle, false>
{
	ReflectedMethodParameterWrapper( const Variant& variant, const IDefinitionManager & )
	{
		ObjectHandle handle = variant.cast<ObjectHandle>();
		pointer = handle;
	}

	ObjectHandle& operator()() { return pointer; }
	ObjectHandle pointer;
};


template<>
struct ReflectedMethodParameterWrapper<ObjectHandle, true>
{
	ReflectedMethodParameterWrapper( const Variant& variant, const IDefinitionManager &)
	{
		ObjectHandle handle = variant.cast<ObjectHandle>();
		pointer = handle;
	}

	ObjectHandle& operator()() { return pointer; }
	ObjectHandle pointer;
};


template<>
struct ReflectedMethodParameterWrapper<Variant, false>
{
	ReflectedMethodParameterWrapper( const Variant& variant, const IDefinitionManager &)
		: variant( variant )
	{}

	Variant& operator()() { return variant; }
	Variant variant;
};


template<>
struct ReflectedMethodParameterWrapper<Variant, true>
{
	ReflectedMethodParameterWrapper( const Variant& variant, const IDefinitionManager &)
		: variant( variant )
	{}

	const Variant& operator()() { return variant; }
	const Variant& variant;
};


#define RM_PARAMETER_NONE( ... )
#define RM_PARAMETER_0( macro ) macro( 0 )
#define RM_PARAMETER_1( macro ) RM_PARAMETER_0( macro ), macro( 1 )
#define RM_PARAMETER_2( macro ) RM_PARAMETER_1( macro ), macro( 2 )
#define RM_PARAMETER_3( macro ) RM_PARAMETER_2( macro ), macro( 3 )
#define RM_PARAMETER_4( macro ) RM_PARAMETER_3( macro ), macro( 4 )
#define RM_PARAMETER_5( macro ) RM_PARAMETER_4( macro ), macro( 5 )
#define RM_PARAMETER_6( macro ) RM_PARAMETER_5( macro ), macro( 6 )
#define RM_PARAMETER_7( macro ) RM_PARAMETER_6( macro ), macro( 7 )
#define RM_PARAMETER_8( macro ) RM_PARAMETER_7( macro ), macro( 8 )
#define RM_PARAMETER_9( macro ) RM_PARAMETER_8( macro ), macro( 9 )

#define RM_PARAMETER_WITH_PREFIX_NONE( prefix, ... ) prefix()
#define RM_PARAMETER_WITH_PREFIX_0( prefix, macro ) prefix(), macro( 0 )
#define RM_PARAMETER_WITH_PREFIX_1( prefix, macro ) RM_PARAMETER_WITH_PREFIX_0( prefix, macro ), macro( 1 )
#define RM_PARAMETER_WITH_PREFIX_2( prefix, macro ) RM_PARAMETER_WITH_PREFIX_1( prefix, macro ), macro( 2 )
#define RM_PARAMETER_WITH_PREFIX_3( prefix, macro ) RM_PARAMETER_WITH_PREFIX_2( prefix, macro ), macro( 3 )
#define RM_PARAMETER_WITH_PREFIX_4( prefix, macro ) RM_PARAMETER_WITH_PREFIX_3( prefix, macro ), macro( 4 )
#define RM_PARAMETER_WITH_PREFIX_5( prefix, macro ) RM_PARAMETER_WITH_PREFIX_4( prefix, macro ), macro( 5 )
#define RM_PARAMETER_WITH_PREFIX_6( prefix, macro ) RM_PARAMETER_WITH_PREFIX_5( prefix, macro ), macro( 6 )
#define RM_PARAMETER_WITH_PREFIX_7( prefix, macro ) RM_PARAMETER_WITH_PREFIX_6( prefix, macro ), macro( 7 )
#define RM_PARAMETER_WITH_PREFIX_8( prefix, macro ) RM_PARAMETER_WITH_PREFIX_7( prefix, macro ), macro( 8 )
#define RM_PARAMETER_WITH_PREFIX_9( prefix, macro ) RM_PARAMETER_WITH_PREFIX_8( prefix, macro ), macro( 9 )

#define RM_STATEMENT_NONE( ... )
#define RM_STATEMENT_0( macro ) macro(0)
#define RM_STATEMENT_1( macro ) RM_STATEMENT_0( macro ) macro( 1 )
#define RM_STATEMENT_2( macro ) RM_STATEMENT_1( macro ) macro( 2 )
#define RM_STATEMENT_3( macro ) RM_STATEMENT_2( macro ) macro( 3 )
#define RM_STATEMENT_4( macro ) RM_STATEMENT_3( macro ) macro( 4 )
#define RM_STATEMENT_5( macro ) RM_STATEMENT_4( macro ) macro( 5 )
#define RM_STATEMENT_6( macro ) RM_STATEMENT_5( macro ) macro( 6 )
#define RM_STATEMENT_7( macro ) RM_STATEMENT_6( macro ) macro( 7 )
#define RM_STATEMENT_8( macro ) RM_STATEMENT_7( macro ) macro( 8 )
#define RM_STATEMENT_9( macro ) RM_STATEMENT_8( macro ) macro( 9 )

#define RM_EXPAND( ... ) __VA_ARGS__
#define RM_JOIN( a, b ) RM_EXPAND( a##b )

#define RM_CLASS_PARAMETER_PREFIX() class ClassType, class ReturnType
#define RM_CLASS_PARAMETERS( n ) \
	RM_JOIN( RM_PARAMETER_WITH_PREFIX_, n )( RM_CLASS_PARAMETER_PREFIX, RM_CLASS_PARAMETER )
#define RM_CLASS_PARAMETERS_PASSED( n ) RM_JOIN( RM_PARAMETER_, n )( RM_CLASS_PARAMETER )
#define RM_CLASS_PARAMETER( n ) class Parameter##n##Type
#define RM_CLASS_PARAMETER_WITH_DEFAULTS( n ) class Parameter##n##Type = void
#define RM_CLASS_PARAMETERS_WITH_DEFAULTS( n )\
	RM_JOIN( RM_PARAMETER_WITH_PREFIX_, n )( RM_CLASS_PARAMETER_PREFIX, RM_CLASS_PARAMETER_WITH_DEFAULTS )

#define RM_PLAIN_PARAMETER_PREFIX() ClassType, ReturnType
#define RM_PLAIN_PARAMETERS( n ) \
	RM_JOIN( RM_PARAMETER_WITH_PREFIX_, n )( RM_PLAIN_PARAMETER_PREFIX, RM_PLAIN_PARAMETER )
#define RM_PLAIN_PARAMETERS_PASSED( n ) RM_JOIN( RM_PARAMETER_, n )( RM_PLAIN_PARAMETER )
#define RM_PLAIN_PARAMETER( n ) Parameter##n##Type

#define RM_PASS_PARAMETER_PREFIX() std::ref( target )
#define RM_PASS_PARAMETERS( n ) RM_JOIN( RM_PARAMETER_WITH_PREFIX_, n )( RM_PASS_PARAMETER_PREFIX, RM_PASS_PARAMETER )
#define RM_PASS_PARAMETER( n ) std::ref( p##n() )

#define RM_EXTRACT_PARAMETER_LINE( n )\
ReflectedMethodParameterWrapper<typename std::decay<Parameter##n##Type>::type,\
std::is_reference<Parameter##n##Type>::value> p##n( parameters[n], definitionManager );

#define RM_EXTRACT_PARAMETER_LINES( n ) RM_JOIN( RM_STATEMENT_, n )( RM_EXTRACT_PARAMETER_LINE )

#define RM_METHOD_SPECIALISATION_BEGIN_DEFAULT( n )\
	template<RM_CLASS_PARAMETERS_WITH_DEFAULTS( n )>\
struct ReflectedMethodSpecialisation\
	RM_METHOD_SPECIALISATION_BEGIN_BODY( n )

#define RM_METHOD_SPECIALISATION_BEGIN( n )\
	template<RM_CLASS_PARAMETERS( n )>\
struct ReflectedMethodSpecialisation<RM_PLAIN_PARAMETERS( n )>\
	RM_METHOD_SPECIALISATION_BEGIN_BODY( n )

#define RM_METHOD_SPECIALISATION_BEGIN_BODY( n )\
	: public MethodReturnSplitter<ReturnType>, public ReflectedMethod\
	{\
	typedef ReturnType( ClassType::*MethodType )( RM_PLAIN_PARAMETERS_PASSED( n ) );\
	typedef void( ClassType::*MethodUndoRedoType )( const ObjectHandle&, Variant );\
	\
	ReflectedMethodSpecialisation( const char* name, MethodType method, MethodUndoRedoType undoMethod, MethodUndoRedoType redoMethod )\
	: ReflectedMethod( name ), method_( method )\
	{\
		undoMethod_.reset( undoMethod \
			? new ReflectedMethodSpecialisation<ClassType, void, const ObjectHandle&, Variant>( name, undoMethod, nullptr, nullptr ) \
			: nullptr );\
		redoMethod_.reset( redoMethod \
			? new ReflectedMethodSpecialisation<ClassType, void, const ObjectHandle&, Variant>( name, redoMethod, nullptr, nullptr ) \
			: nullptr );\
	}\
	\
	Variant invoke(const ObjectHandle& object,\
		const IDefinitionManager & definitionManager,\
		const ReflectedMethodParameters& parameters) override\
		{\
		auto pointer = reflectedCast< ClassType >( object.data(), object.type(), definitionManager );\
		assert( pointer!= nullptr );

#define RM_METHOD_SPECIALISATION_END( count )\
	}\
	\
	size_t parameterCount() const override { return count; }\
	ReflectedMethod* getUndoMethod() override { return undoMethod_.get(); }\
	ReflectedMethod* getRedoMethod() override { return redoMethod_.get(); }\
	\
	MethodType method_;\
	std::unique_ptr<ReflectedMethod> undoMethod_;\
	std::unique_ptr<ReflectedMethod> redoMethod_;\
};


#define RM_FACTORY_CREATE_METHOD( n )\
	template<RM_CLASS_PARAMETERS( n )>\
	static ReflectedMethod* create( const char* name,\
		ReturnType( ClassType::*method )( RM_PLAIN_PARAMETERS_PASSED( n ) ),\
		typename ReflectedMethodSpecialisation<RM_PLAIN_PARAMETERS(n)>::MethodUndoRedoType undoMethod,\
		typename ReflectedMethodSpecialisation<RM_PLAIN_PARAMETERS(n)>::MethodUndoRedoType redoMethod)\
	{ return new ReflectedMethodSpecialisation<RM_PLAIN_PARAMETERS( n )>( name, method, undoMethod, redoMethod ); }


RM_METHOD_SPECIALISATION_BEGIN_DEFAULT( 9 )
		ClassType& target = *pointer;
		RM_EXTRACT_PARAMETER_LINES( 9 );
		this->methodWithoutParameters = std::bind( method_, RM_PASS_PARAMETERS( 9 ) );
		return MethodReturnSplitter<ReturnType>::invoke();
RM_METHOD_SPECIALISATION_END( 10 )


RM_METHOD_SPECIALISATION_BEGIN( 8 )
		ClassType& target = *pointer;
		RM_EXTRACT_PARAMETER_LINES( 8 )
		this->methodWithoutParameters = std::bind( method_, RM_PASS_PARAMETERS( 8 ) );
		return MethodReturnSplitter<ReturnType>::invoke();
RM_METHOD_SPECIALISATION_END( 9 )


RM_METHOD_SPECIALISATION_BEGIN( 7 )
		ClassType& target = *pointer;
		RM_EXTRACT_PARAMETER_LINES( 7 )
		this->methodWithoutParameters = std::bind( method_, RM_PASS_PARAMETERS( 7 ) );
		return MethodReturnSplitter<ReturnType>::invoke();
RM_METHOD_SPECIALISATION_END( 8 )


RM_METHOD_SPECIALISATION_BEGIN( 6 )
		ClassType& target = *pointer;
		RM_EXTRACT_PARAMETER_LINES( 6 )
		this->methodWithoutParameters = std::bind( method_, RM_PASS_PARAMETERS( 6 ) );
		return MethodReturnSplitter<ReturnType>::invoke();
RM_METHOD_SPECIALISATION_END( 7 )


RM_METHOD_SPECIALISATION_BEGIN( 5 )
		ClassType& target = *pointer;
		RM_EXTRACT_PARAMETER_LINES( 5 )
		this->methodWithoutParameters = std::bind( method_, RM_PASS_PARAMETERS( 5 ) );
		return MethodReturnSplitter<ReturnType>::invoke();
RM_METHOD_SPECIALISATION_END( 6 )


RM_METHOD_SPECIALISATION_BEGIN( 4 )
		ClassType& target = *pointer;
		RM_EXTRACT_PARAMETER_LINES( 4 )
		this->methodWithoutParameters = std::bind( method_, RM_PASS_PARAMETERS( 4 ) );
		return MethodReturnSplitter<ReturnType>::invoke();
RM_METHOD_SPECIALISATION_END( 5 )


RM_METHOD_SPECIALISATION_BEGIN( 3 )
		ClassType& target = *pointer;
		RM_EXTRACT_PARAMETER_LINES( 3 )
		this->methodWithoutParameters = std::bind( method_, RM_PASS_PARAMETERS( 3 ) );
		return MethodReturnSplitter<ReturnType>::invoke();
RM_METHOD_SPECIALISATION_END( 4 )


RM_METHOD_SPECIALISATION_BEGIN( 2 )
		ClassType& target = *pointer;
		RM_EXTRACT_PARAMETER_LINES( 2 )
		this->methodWithoutParameters = std::bind( method_, RM_PASS_PARAMETERS( 2 ) );
		return MethodReturnSplitter<ReturnType>::invoke();
RM_METHOD_SPECIALISATION_END( 3 )


RM_METHOD_SPECIALISATION_BEGIN( 1 )
		ClassType& target = *pointer;
		RM_EXTRACT_PARAMETER_LINES( 1 )
		this->methodWithoutParameters = std::bind( method_, RM_PASS_PARAMETERS( 1 ) );
		return MethodReturnSplitter<ReturnType>::invoke();
RM_METHOD_SPECIALISATION_END( 2 )


RM_METHOD_SPECIALISATION_BEGIN( 0 )
		ClassType& target = *pointer;
		RM_EXTRACT_PARAMETER_LINES( 0 )
		this->methodWithoutParameters = std::bind( method_, RM_PASS_PARAMETERS( 0 ) );
		return MethodReturnSplitter<ReturnType>::invoke();
RM_METHOD_SPECIALISATION_END( 1 )


RM_METHOD_SPECIALISATION_BEGIN( NONE )
		ClassType& target = *pointer;
		RM_EXTRACT_PARAMETER_LINES( NONE )
		this->methodWithoutParameters = std::bind( method_, RM_PASS_PARAMETERS( NONE ) );
		return MethodReturnSplitter<ReturnType>::invoke();
RM_METHOD_SPECIALISATION_END( 0 )

struct ReflectedMethodFactory
{
	RM_FACTORY_CREATE_METHOD( 9 )
	RM_FACTORY_CREATE_METHOD( 8 )
	RM_FACTORY_CREATE_METHOD( 7 )
	RM_FACTORY_CREATE_METHOD( 6 )
	RM_FACTORY_CREATE_METHOD( 5 )
	RM_FACTORY_CREATE_METHOD( 4 )
	RM_FACTORY_CREATE_METHOD( 3 )
	RM_FACTORY_CREATE_METHOD( 2 )
	RM_FACTORY_CREATE_METHOD( 1 )
	RM_FACTORY_CREATE_METHOD( 0 )
	RM_FACTORY_CREATE_METHOD( NONE )
};
} // end namespace wgt
#endif //REFLECTION_METHOD_UTILITIES_HPP
