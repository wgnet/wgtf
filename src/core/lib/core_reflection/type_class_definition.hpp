#ifndef TYPE_CLASS_DEFINITION_HPP
#define TYPE_CLASS_DEFINITION_HPP

#include "interfaces/i_class_definition_details.hpp"
#include "interfaces/i_class_definition_modifier.hpp"
#include "utilities/definition_helpers.hpp"
#include "metadata/meta_impl.hpp"
#include "metadata/meta_utilities.hpp"
#include "property_storage.hpp"
#include <memory>

namespace wgt
{
template< typename Type >
class TypeClassDefinition
	: public IClassDefinitionDetails
{
	typedef Type SelfType;

	MetaHandle metaData_;
	const char * parentName_;

public:
	TypeClassDefinition();

	void * upCast( void * object ) const override;

	//--------------------------------------------------------------------------
	bool isAbstract() const override
	{
		return std::is_abstract< Type >();
	}


	//--------------------------------------------------------------------------
	bool isGeneric() const override
	{
		return false;
	}


	//--------------------------------------------------------------------------
	const char * getName() const override
	{
		return getClassIdentifier< Type >();
	}


	//--------------------------------------------------------------------------
	const char * getParentName() const override
	{
		return parentName_;
	}
	

	//--------------------------------------------------------------------------
	MetaHandle getMetaData() const override
	{
		return metaData_;
	}


	//--------------------------------------------------------------------------
	PropertyIteratorImplPtr getPropertyIterator() const override
	{
		return properties_.getIterator();
	}


	//--------------------------------------------------------------------------
	IClassDefinitionModifier * getDefinitionModifier() const override
	{
		return nullptr;
	}


	//--------------------------------------------------------------------------
	ObjectHandle create( const IClassDefinition & definition ) const override
	{
		auto pInst = std::unique_ptr< Type >( CreateHelper< Type >::create() );
		return ObjectHandle( std::move( pInst ), &definition );
	}

	//--------------------------------------------------------------------------
	template<class TArg1>
	static ObjectHandleT<Type> create(const IClassDefinition & definition, TArg1&& arg)
	{
		auto pInst = std::unique_ptr< Type >( CreateHelper< Type >::create(
			std::forward<TArg1>(arg) ) );
		return safeCast< Type >( ObjectHandle(std::move(pInst), &definition) );
	}

	//--------------------------------------------------------------------------
	template<class TArg1, class TArg2>
	static ObjectHandleT<Type> create(const IClassDefinition & definition, TArg1&& arg, TArg2&& arg2)
	{
		auto pInst = std::unique_ptr< Type >( CreateHelper< Type >::create(
			std::forward<TArg1>(arg), std::forward<TArg2>(arg2) ) );
		return safeCast< Type >( ObjectHandle(std::move(pInst), &definition) );
	}

	//--------------------------------------------------------------------------
	template<class TArg1, class TArg2, class TArg3>
	static ObjectHandleT<Type> create(const IClassDefinition & definition,
		TArg1&& arg, TArg2&& arg2, TArg3&& arg3)
	{
		auto pInst = std::unique_ptr< Type >( CreateHelper< Type >::create(
			std::forward<TArg1>(arg), std::forward<TArg1>(arg2), std::forward<TArg3>(arg3) ) );
		return safeCast< Type >( ObjectHandle(std::move(pInst), &definition) );
	}

	//--------------------------------------------------------------------------
	template<class TArg1, class TArg2, class TArg3, class TArg4>
	static ObjectHandleT<Type> create(const IClassDefinition & definition,
		TArg1&& arg, TArg2&& arg2, TArg3&& arg3, TArg4&& arg4)
	{
		auto pInst = std::unique_ptr< Type >( CreateHelper< Type >::create(
			std::forward<TArg1>(arg), std::forward<TArg2>(arg2), std::forward<TArg3>(arg3),
			std::forward<TArg4>(arg4 ) ) );
		return safeCast< Type >( ObjectHandle(std::move(pInst), &definition) );
	}

	//--------------------------------------------------------------------------
	template<class TArg1, class TArg2, class TArg3, class TArg4, class TArg5>
	static ObjectHandleT<Type> create(const IClassDefinition & definition,
		TArg1&& arg, TArg2&& arg2, TArg3&& arg3, TArg4&& arg4, TArg5&& arg5)
	{
		auto pInst = std::unique_ptr< Type >( CreateHelper< Type >::create(
			std::forward<TArg1>(arg), std::forward<TArg2>(arg2), std::forward<TArg3>(arg3),
			std::forward<TArg4>(arg4), std::forward<TArg5>(arg5) ) );
		return safeCast< Type >( ObjectHandle(std::move(pInst), &definition) );
	}

	//--------------------------------------------------------------------------
	template<class TArg1, class TArg2, class TArg3, class TArg4, class TArg5, class TArg6>
	static ObjectHandleT<Type> create(const IClassDefinition & definition,
		TArg1&& arg, TArg2&& arg2, TArg3&& arg3, TArg4&& arg4, TArg5&& arg5, TArg6&& arg6)
	{
		auto pInst = std::unique_ptr< Type >( CreateHelper< Type >::create(
			std::forward<TArg1>(arg), std::forward<TArg2>(arg2), std::forward<TArg3>(arg3),
			std::forward<TArg4>(arg4), std::forward<TArg5>(arg5), std::forward<TArg6>(arg6) ) );
		return safeCast< Type >( ObjectHandle(std::move(pInst), &definition) );
	}

private:
	PropertyStorage properties_;
};
} // end namespace wgt
#endif // #define TYPE_CLASS_DEFINITION_HPP
