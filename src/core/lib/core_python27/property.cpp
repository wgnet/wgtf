#include "pch.hpp"
#include "property.hpp"

#include "defined_instance.hpp"
#include "type_converters/converter_queue.hpp"
#include "type_converters/i_type_converter.hpp"

#include "core_dependency_system/depends.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/reflected_method_parameters.hpp"
#include "type_converters/converters.hpp"

#include "wg_pyscript/py_script_object.hpp"
#include "wg_types/hash_utilities.hpp"


namespace wgt
{
namespace ReflectedPython
{


typedef Depends< PythonType::Converters  > ImplementationDepends;
class Property::Implementation
	: public ImplementationDepends
{
public:
	Implementation( IComponentContext & context,
		const char * key,
		const PyScript::ScriptObject & pythonObject );

	Implementation( IComponentContext & context,
		const char * key,
		const TypeId & typeId,
		const PyScript::ScriptObject & pythonObject );

	bool setValue( const Variant & value );
	Variant getValue( const ObjectHandle & handle );


	// Need to store a copy of the string
	std::string key_;
	TypeId type_;
	PyScript::ScriptObject pythonObject_;
	uint64_t hash_;
};


Property::Implementation::Implementation( IComponentContext & context,
	const char * key,
	const PyScript::ScriptObject & pythonObject )
	: ImplementationDepends( context )
	, key_( key )
	, pythonObject_( pythonObject )
	, hash_( HashUtilities::compute( key_ ) )
{
	const auto attribute = pythonObject_.getAttribute( key_.c_str(),
		PyScript::ScriptErrorPrint() );
	assert( attribute.exists() );
	type_ = PythonType::scriptTypeToTypeId( attribute );
}


Property::Implementation::Implementation( IComponentContext & context,
	const char * key,
	const TypeId & typeId,
	const PyScript::ScriptObject & pythonObject )
	: ImplementationDepends( context )
	, key_( key )
	, type_( typeId )
	, pythonObject_( pythonObject )
	, hash_( HashUtilities::compute( key_ ) )
{
	// TODO: set a default value of type_ on the attribute
}


bool Property::Implementation::setValue( const Variant & value )
{
	auto pTypeConverters = get< PythonType::Converters >();
	assert( pTypeConverters != nullptr );

	PyScript::ScriptObject scriptObject;
	const bool success = pTypeConverters->toScriptType( value, scriptObject );
	assert( success );
	PyScript::ScriptErrorPrint errorHandler;
	if (!pythonObject_.setAttribute( key_.c_str(),
		scriptObject,
		errorHandler ))
	{
		return false;
	}

	type_ = PythonType::scriptTypeToTypeId( scriptObject );
	return true;
}


Variant Property::Implementation::getValue( const ObjectHandle & handle )
{
#if defined( _DEBUG )
	auto pInstance = handle.getBase< DefinedInstance >();
	assert( pInstance != nullptr );
	assert( pInstance->pythonObject().compareTo( pythonObject_,
		PyScript::ScriptErrorPrint() ) == 0 );
#endif // defined( _DEBUG )

	PyScript::ScriptErrorPrint errorHandler;

	// Get the attribute
	PyScript::ScriptObject attribute = pythonObject_.getAttribute(
		key_.c_str(),
		errorHandler );

	auto pTypeConverters = get< PythonType::Converters >();
	assert( pTypeConverters != nullptr );

	Variant value;
	const bool success = pTypeConverters->toVariant( attribute, value, handle, key_ );
	assert( success );
	return value;
}


Property::Property( IComponentContext & context,
	const char * key,
	const PyScript::ScriptObject & pythonObject )
	: IBaseProperty()
	, impl_( new Implementation( context, key, pythonObject ) )
{
}


Property::Property( IComponentContext & context,
	const char * key,
	const TypeId & typeId,
	const PyScript::ScriptObject & pythonObject )
	: IBaseProperty()
	, impl_( new Implementation( context, key, typeId, pythonObject ) )
{
}


const TypeId & Property::getType() const /* override */
{
	return impl_->type_;
}


const char * Property::getName() const /* override */
{
	return impl_->key_.c_str();
}


uint64_t Property::getNameHash() const /* override */
{
	return impl_->hash_;
}


MetaHandle Property::getMetaData() const /* override */
{
	return nullptr;
}


bool Property::readOnly() const /* override */
{
	// Python uses EAFP, so it can't check if a property is read-only before
	// trying to set it.
	// Have to try to set and check for an exception.
	// https://docs.python.org/2/glossary.html#term-eafp
	return false;
}


bool Property::isMethod() const /* override */
{
	// Get the attribute
	PyScript::ScriptErrorPrint errorHandler;
	PyScript::ScriptObject attribute = impl_->pythonObject_.getAttribute(
		impl_->key_.c_str(),
		errorHandler );
	assert( attribute.exists() );
	if (!attribute.exists())
	{
		return false;
	}

	// Checks if the attribute is "callable", it may be:
	// - an instance with a __call__ attribute
	// or
	// - a type with a tp_call member, such as
	// -- a method on a class
	// -- a function/lambda type
	return attribute.isCallable();
}


bool Property::isValue() const /* override */
{
	// Attribute must exist
	return true;
}


bool Property::set( const ObjectHandle & handle,
	const Variant & value,
	const IDefinitionManager & definitionManager ) const /* override */
{
	return impl_->setValue( value );
}


Variant Property::get( const ObjectHandle & handle,
	const IDefinitionManager & definitionManager ) const /* override */
{
	return impl_->getValue( handle );
}


Variant Property::invoke( const ObjectHandle& object,
	const IDefinitionManager & definitionManager,
	const ReflectedMethodParameters& parameters ) /* override */
{
	const bool callable = this->isMethod();
	assert( callable );
	if (!callable)
	{
		return Variant();
	}

	auto pTypeConverters = impl_->get< PythonType::Converters >();
	assert( pTypeConverters != nullptr );

	// Parse arguments
	auto tuple = PyScript::ScriptTuple::create( parameters.size() );
	size_t i = 0;

	for (auto itr = parameters.cbegin();
		(i < parameters.size()) && (itr != parameters.cend());
		++i, ++itr)
	{
		auto parameter = (*itr);
		PyScript::ScriptObject scriptObject;
		const bool success = pTypeConverters->toScriptType( parameter, scriptObject );
		assert( success );
		tuple.setItem( i, scriptObject );
	}

	PyScript::ScriptArgs args = PyScript::ScriptArgs( tuple.get(),
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE );

	// Call method
	PyScript::ScriptErrorPrint errorHandler;
	const bool allowNullMethod = false;
	PyScript::ScriptObject returnValue = impl_->pythonObject_.callMethod(
		impl_->key_.c_str(),
		args,
		errorHandler,
		allowNullMethod );

	// Return value
	Variant result;

	if (returnValue.exists())
	{
		const bool success = pTypeConverters->toVariant( returnValue,
			result,
			object,
			impl_->key_ );
		assert( success );
	}

	return result;
}


size_t Property::parameterCount() const /* override */
{
	PyScript::ScriptObject attribute = impl_->pythonObject_.getAttribute(
		impl_->key_.c_str(),
		PyScript::ScriptErrorPrint() );
	assert( attribute.exists() );
	if (!attribute.exists())
	{
		return 0;
	}

	if (!attribute.isCallable())
	{
		return 0;
	}

	// -- Old-style class instance.__call__(self)
	if (PyScript::ScriptInstance::check( attribute ))
	{
		auto callObject = attribute.getAttribute( "__call__", PyScript::ScriptErrorClear() );
		if (!callObject.exists())
		{
			return 0;
		}

		// Convert __call__(self) method object to a function()
		auto methodObject = PyScript::ScriptMethod::create( callObject );
		assert( methodObject.exists() );

		auto functionObject = methodObject.function();
		assert( functionObject.exists() );

		// Convert function to code and get arg count
		auto codeObject = functionObject.code();
		assert( codeObject.exists() );

		const auto argCount = codeObject.argCount();

		// Methods subtract 1 argument for "self".
		const int selfArg = 1;
		assert( argCount > 0 );
		return (argCount - selfArg);
	}

	// -- Old-style class constructor instance(self)
	if (PyScript::ScriptClass::check( attribute ))
	{
		auto initObject = attribute.getAttribute( "__init__", PyScript::ScriptErrorClear() );
		if (!initObject.exists())
		{
			// Default __init__(self)
			return 0;
		}

		// Convert __init__(self) method object to a function()
		auto methodObject = PyScript::ScriptMethod::create( initObject );
		assert( methodObject.exists() );

		auto functionObject = methodObject.function();
		assert( functionObject.exists() );

		// Convert function to code and get arg count
		auto codeObject = functionObject.code();
		assert( codeObject.exists() );

		const auto argCount = codeObject.argCount();

		// Methods subtract 1 argument for "self".
		const int selfArg = 1;
		assert( argCount > 0 );
		return (argCount - selfArg);
	}

	// -- Method like self.function(self)
	auto methodObject = PyScript::ScriptMethod::create( attribute );
	if (methodObject.exists())
	{
		// Convert self.function() method object to a function()
		auto functionObject = methodObject.function();
		assert( functionObject.exists() );

		// Convert function to code and get arg count
		auto codeObject = functionObject.code();
		assert( codeObject.exists() );

		const auto argCount = codeObject.argCount();

		// Methods subtract 1 argument for "self".
		const int selfArg = 1;
		assert( argCount > 0 );
		return (argCount - selfArg);
	}

	// -- Plain function or lambda type
	auto functionObject = PyScript::ScriptFunction::create( attribute );
	if (functionObject.exists())
	{
		auto codeObject = functionObject.code();
		assert( codeObject.exists() );
		return codeObject.argCount();
	}

	// -- New-style class instance.__call__(self)
	auto callObject = attribute.getAttribute( "__call__", PyScript::ScriptErrorClear() );

	// Convert __call__(self) method object to a function()
	methodObject = PyScript::ScriptMethod::create( callObject );
	if (methodObject.exists())
	{
		// Convert function to code and get arg count
		functionObject = methodObject.function();
		assert( functionObject.exists() );

		auto codeObject = functionObject.code();
		assert( codeObject.exists() );

		const auto argCount = codeObject.argCount();

		// Methods subtract 1 argument for "self".
		const int selfArg = 1;
		assert( argCount > 0 );
		return (argCount - selfArg);
	}

	// -- New-style class constructor instance.__init__(self)
	auto initObject = attribute.getAttribute( "__init__", PyScript::ScriptErrorClear() );

	// Convert __init__(self) method object to a function()
	methodObject = PyScript::ScriptMethod::create( initObject );
	if (methodObject.exists())
	{
		// Convert function to code and get arg count
		functionObject = methodObject.function();
		assert( functionObject.exists() );

		auto codeObject = functionObject.code();
		assert( codeObject.exists() );

		const auto argCount = codeObject.argCount();

		// Methods subtract 1 argument for "self".
		const int selfArg = 1;
		assert( argCount > 0 );
		return (argCount - selfArg);
	}

	// Default __init__(self)
	return 0;
}


} // namespace ReflectedPython
} // end namespace wgt
