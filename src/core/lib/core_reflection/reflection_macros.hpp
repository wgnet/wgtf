#ifndef REFLECTION_MACROS_HPP
#define REFLECTION_MACROS_HPP

#include "base_property_with_metadata.hpp"
#include "lambda_property.hpp"
#include "type_class_definition.hpp"
#include "utilities/reflection_method_utilities.hpp"
#include "utilities/counter_helper.hpp"
#include "i_definition_manager.hpp" 
#include "metadata/meta_utilities.hpp"

//Default fallback that does nothing
namespace wgt
{

class IDefinitionManager;

template< size_t >
struct ReflectionCreatorHelper
{
	static wgt::IClassDefinition * autoRegister(IDefinitionManager & definitionManager)
	{
		//Do nothing	
		return nullptr;
	}
};

} //end namespace wgt

//Specialize a creator for this type based on compile-time generated id
#define EXPOSE_CREATOR( type ) \
template<>\
struct ::wgt::ReflectionCreatorHelper< ::wgt::ReflectionAutoRegistration::next() >\
{\
	static wgt::IClassDefinition * autoRegister(::wgt::IDefinitionManager & definitionManager)\
	{\
		return definitionManager.registerDefinition< ::wgt::TypeClassDefinition< type > >( ); \
	}\
};

#define BEGIN_EXPOSE_1(baseSpace)		                                                                 \
	EXPOSE_CREATOR( baseSpace )																			 \
	template <>                                                                                          \
	::wgt::TypeClassDefinition<baseSpace>::TypeClassDefinition() : metaData_(nullptr)					 \
	{

#define BEGIN_EXPOSE_2(baseSpace, meta)                                                                  \
	EXPOSE_CREATOR( baseSpace )																			 \
	template <>                                                                                          \
	::wgt::TypeClassDefinition<baseSpace>::TypeClassDefinition() : metaData_(meta)						 \
	{


//Only for backward compatibility
#define BEGIN_EXPOSE_3(baseSpace, base, meta)										\
	EXPOSE_CREATOR( baseSpace )														\
	template <>																		\
	::wgt::TypeClassDefinition<baseSpace>::TypeClassDefinition()					\
	    : metaData_(meta)															\
	{																				\
			metaData_ += MetaBasesHolder(											\
				std::unique_ptr< IDirectBaseHelper >(								\
					new DirectBaseHelperT< baseSpace, base >()));					\

#define EXPOSE_METHOD_2(name, method) \
	properties_.addProperty(          \
	IBasePropertyPtr(ReflectedMethodFactory::create(name, &SelfType::method, nullptr, nullptr)));

#define EXPOSE_METHOD_3(name, method, meta)                                                                     \
	properties_.addProperty(IBasePropertyPtr(                                                                   \
	new BasePropertyWithMetaData(IBasePropertyPtr(ReflectedMethodFactory::create(                               \
	                             name, &SelfType::method, nullptr /* undoMethod */, nullptr /* redoMethod */)), \
	                             meta)));

#define EXPOSE_METHOD_4(name, method, undoMethod, redoMethod) \
	properties_.addProperty(IBasePropertyPtr(                 \
	ReflectedMethodFactory::create(name, &SelfType::method, &SelfType::undoMethod, &SelfType::redoMethod)));

#define EXPOSE_METHOD_5(name, method, undoMethod, redoMethod, meta)                                       \
	properties_.addProperty(IBasePropertyPtr(                                                             \
	new BasePropertyWithMetaData(IBasePropertyPtr(ReflectedMethodFactory::create(                         \
	                             name, &SelfType::method, &SelfType::undoMethod, &SelfType::redoMethod)), \
	                             meta)));

#define EXPOSE_2(name, _1) \
	properties_.addProperty(IBasePropertyPtr(FunctionPropertyHelper<SelfType>::getBaseProperty(name, &SelfType::_1)));

#define EXPOSE_3(name, _1, meta)                                           \
	properties_.addProperty(IBasePropertyPtr(new BasePropertyWithMetaData( \
	IBasePropertyPtr(FunctionPropertyHelper<SelfType>::getBaseProperty(name, &SelfType::_1)), meta)));

#define EXPOSE_4(name, _1, _2, meta)                                       \
	properties_.addProperty(IBasePropertyPtr(new BasePropertyWithMetaData( \
	IBasePropertyPtr(FunctionPropertyHelper<SelfType>::getBaseProperty(name, &SelfType::_1, &SelfType::_2)), meta)));

#define EXPOSE_5(name, _1, _2, _3, meta)                                                                              \
	properties_.addProperty(                                                                                          \
	IBasePropertyPtr(new BasePropertyWithMetaData(IBasePropertyPtr(FunctionPropertyHelper<SelfType>::getBaseProperty( \
	                                              name, &SelfType::_1, &SelfType::_2, &SelfType::_3)),                \
	                                              meta)));

#define EXPOSE_6(name, _1, _2, _3, _4, meta)                                                                          \
	properties_.addProperty(                                                                                          \
	IBasePropertyPtr(new BasePropertyWithMetaData(IBasePropertyPtr(FunctionPropertyHelper<SelfType>::getBaseProperty( \
	                                              name, &SelfType::_1, &SelfType::_2, &SelfType::_3, &SelfType::_4)), \
	                                              meta)));

#define EXPOSE_CALLABLE(...) properties_.addProperty(LambdaProperty::create<SelfType>(__VA_ARGS__));

// Example of expansion
//
// 4 arguments
// EXPOSE( a, b, c, d )
// MACRO_CHOOSER_( NUM_ARGS_( a, b, c, d ) ) ( a, b, c, d )
// EXPOSE_( NUM_ARGS__( ( a, b, c, d ), REVERSE_SEQ_NUM_() ) ) ) ( a, b, c, d )
// EXPOSE_##( STRIP_USELESS_ARGS_( a, b, c, d, 4, 3, 2, 1, 0 ) ) ) ( a, b, c, d )
// EXPOSE_##4( a, b, c, d )
// EXPOSE_4( a, b, c, d )
//
//
// 3 arguments
// EXPOSE( a, b, c )
// MACRO_CHOOSER( NUM_ARGS_( a, b, c ) )( a, b, c )
// EXPOSE_( NUM_ARGS__( ( a, b, c ), REVERSE_SEQ_NUM() ) ) )( a, b, c )
// EXPOSE_##( STRIP_USELESS_ARGS_( a, b, c, 4, 3, 2, 1, 0 ) ) )( a, b, c )
// EXPOSE_##( 3 )( a, b, c )
// EXPOSE_3( a, b, c )

// We need this because the Msft compiler tries to expand one step too early
// and causes the __VA_ARGS__ to be calculated wrongly, so we add one level of indirection
#define EXPAND_(X) X

#define REVERSE_SEQ_NUM_() 6, 5, 4, 3, 2, 1, 0
#define STRIP_USELESS_ARGS_(_1, _2, _3, _4, _5, _6, N, ...) N

#define NUM_ARGS__(...) EXPAND_(STRIP_USELESS_ARGS_(__VA_ARGS__))
#define NUM_ARGS_(...) EXPAND_(NUM_ARGS__(__VA_ARGS__, REVERSE_SEQ_NUM_()))

#define EXPOSE_(N) EXPOSE_##N
#define MACRO_CHOOSER_(N) EXPOSE_(N)
#define EXPOSE(...) EXPAND_(MACRO_CHOOSER_(EXPAND_(NUM_ARGS_(__VA_ARGS__)))(__VA_ARGS__)) // Real macro arguments

#define EXPOSE_METHOD_(N) EXPOSE_METHOD_##N
#define METHOD_MACRO_CHOOSER_(N) EXPOSE_METHOD_(N)
#define EXPOSE_METHOD(...) \
	EXPAND_(METHOD_MACRO_CHOOSER_(EXPAND_(NUM_ARGS_(__VA_ARGS__)))(__VA_ARGS__)) // Real macro arguments

#define BEGIN_EXPOSE_(N) BEGIN_EXPOSE_##N
#define MACRO_CHOOSER_2(N) BEGIN_EXPOSE_(N)
#define BEGIN_EXPOSE(...) EXPAND_(MACRO_CHOOSER_2(EXPAND_(NUM_ARGS_(__VA_ARGS__)))(__VA_ARGS__)) // Real macro arguments

#define END_EXPOSE() }; /* End definition */

#define BEGIN_EXPOSE_ENUM(TEnum) \
	const std::map<typename std::underlying_type<TEnum>::type, std::string> wgt::EnumGenerator<TEnum, true>::enums_	 = \
	(std::map<typename std::underlying_type<TEnum>::type, std::string>&&) std::map<TEnum, std::string>{

#endif // REFLECTION_MACROS_HPP
