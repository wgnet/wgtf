#include "pch.hpp"

#include "listener_hooks.hpp"

#include "defined_instance.hpp"

#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_script/type_converter_queue.hpp"
#include "core_reflection/property_accessor.hpp"
#include "type_converters/converters.hpp"
#include "wg_pyscript/py_script_object.hpp"


namespace wgt
{
IComponentContext * g_pHookContext = nullptr;
std::weak_ptr< ReflectedPython::HookListener > g_listener;


#if ENABLE_PYTHON_LISTENER_HOOKS


/**
 *	Hook for listening to property changes.
 *
 *	Python functions should stay in the global namespace.
 *
 *	@param self Python object.
 *	@param name of attribute to be set.
 *		May add a new attribute if one does not exist.
 *	@param value to be set on the attribute.
 *	@return -1 on error.
 */
static PyObject * wrap_setattr( PyObject * self, PyObject * args, void * wrapped )
{
	PyObject * name;
	PyObject * value;
	if (!PyArg_UnpackTuple( args, "", 2, 2, &name, &value ))
	{
		return nullptr;
	}
	assert( wrapped != nullptr );

	auto selfObject = PyScript::ScriptObject( self,
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
	auto typeObject = PyScript::ScriptType::getType( selfObject );

	// -- Check if listeners do not need to be notified
	// If the HookListener has already been entered, then this property must
	// have been set by the command system and we do not need to fire another
	// notification.
	// If it has not been entered, then this property must have been set by
	// a script.
	assert( !g_listener.expired() );
	auto listener = g_listener.lock();
	if (listener->entered())
	{
		// -- Set attribute using default hook
		// __setattr__( name, value )
		setattrofunc func = static_cast< setattrofunc >( wrapped );
		const int res = (*func)(self, name, value);
		if (res < 0)
		{
			return nullptr;
		}
		Py_RETURN_NONE;
	}

	// -- Check if listeners need to be notified
	assert( g_pHookContext != nullptr );
	auto & context = (*g_pHookContext);
	auto handle = ReflectedPython::DefinedInstance::find( context, selfObject );

	// selfObject should always be found in the reflection system
	// because the hook should be attached & detached when the object
	// enters & leaves the system.
	// But the setattr hook may be triggered by setting an attribute on
	// a reflected object's *type*. So just ignore this as the type is not
	// tracked by the reflection system.
	if (!handle.isValid())
	{
		// -- Set attribute using default hook
		// __setattr__( name, value )
		setattrofunc func = static_cast< setattrofunc >( wrapped );
		const int res = (*func)(self, name, value);
		if (res < 0)
		{
			return nullptr;
		}
		Py_RETURN_NONE;
	}

	// -- Pre-notify UI
	// TODO support preItemsInserted/postItemsInserted
	auto tmpName = PyScript::ScriptObject( name,
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE );
	auto nameObject = PyScript::ScriptString::create( tmpName );
	if (!nameObject.exists())
	{
		PyErr_Format( PyExc_AttributeError,
			"Cannot set attribute with no name." );
		return nullptr;
	}
	auto valueObject = PyScript::ScriptObject( value,
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE );


	auto pDefinedInstance = handle.getBase< ReflectedPython::DefinedInstance >();
	assert( pDefinedInstance != nullptr );
	auto & definedInstance = (*pDefinedInstance);

	auto pTypeConverters = context.queryInterface< PythonType::Converters >();
	assert( pTypeConverters != nullptr );

	auto pDefinitionManager = context.queryInterface< IDefinitionManager >();
	assert( pDefinitionManager != nullptr );

#if ENABLE_FULL_PATH_PYTHON_LISTENER_HOOKS
	// Get root object to construct PropertyAccessor.
	// PropertyAccessorListener requires a (root object, full path) pair to
	// detect changes.
	const auto & rootInstance = definedInstance.root();

	// Find existing ObjectHandle corresponding to instance
	auto rootHandle = ReflectedPython::DefinedInstance::find( context,
		rootInstance.pythonObject() );

	const std::string childPath( nameObject.c_str() );
	std::string fullPath( definedInstance.fullPath() );
	if (!fullPath.empty())
	{
		fullPath += DOT_OPERATOR;
	}
	fullPath += childPath;

	auto pDefinition = rootInstance.getDefinition();
	assert( pDefinition != nullptr );
	auto propertyAccessor = pDefinition->bindProperty( fullPath.c_str(), rootHandle );

#else // ENABLE_FULL_PATH_PYTHON_LISTENER_HOOKS

	const std::string childPath( nameObject.c_str() );
	auto pDefinition = definedInstance.getDefinition();
	assert( pDefinition != nullptr );
	auto propertyAccessor = pDefinition->bindProperty( childPath.c_str(), handle );
#endif // ENABLE_FULL_PATH_PYTHON_LISTENER_HOOKS

	Variant variantValue;
	const bool success = pTypeConverters->toVariant( valueObject,
		variantValue,
		selfObject,
		childPath );
	assert( success );

	auto& listeners = pDefinitionManager->getPropertyAccessorListeners();
	const auto itBegin = listeners.cbegin();
	const auto itEnd = listeners.cend();
	for (auto it = itBegin; it != itEnd; ++it)
	{
		auto listener = it->lock();
		assert( listener != nullptr );
		listener->preSetValue( propertyAccessor, variantValue );
	}

	// -- Set attribute using default hook
	// __setattr__( name, value )
	setattrofunc func = static_cast< setattrofunc >( wrapped );
	const int res = (*func)(self, name, value);

	// -- Post-notify UI
	for (auto it = itBegin; it != itEnd; ++it)
	{
		auto listener = it->lock();
		assert( listener != nullptr );
		listener->postSetValue( propertyAccessor, variantValue );
	}

	if (res < 0)
	{
		return nullptr;
	}
	Py_RETURN_NONE;
}


/**
 *	Copied from typeobject.c.
 */
static int
check_num_args(PyObject *ob, int n)
{
	if (!PyTuple_CheckExact(ob)) {
		PyErr_SetString(PyExc_SystemError,
			"PyArg_UnpackTuple() argument list is not a tuple");
		return 0;
	}
	if (n == PyTuple_GET_SIZE(ob))
		return 1;
	PyErr_Format(
		PyExc_TypeError,
		"expected %d arguments, got %zd", n, PyTuple_GET_SIZE(ob));
	return 0;
}


/**
 *	Copied from typeobject.c.
 *	TODO notify listeners of add/remove
 */
static PyObject * wrap_delattr(PyObject *self, PyObject *args, void *wrapped)
{
	setattrofunc func = (setattrofunc)wrapped;
	int res;
	PyObject *name;

	if (!check_num_args(args, 1))
		return NULL;
	name = PyTuple_GET_ITEM(args, 0);
	res = (*func)(self, name, NULL);
	if (res < 0)
		return NULL;
	Py_INCREF(Py_None);
	return Py_None;
}


/**
 *	Copied from typeobject.c.
 *	Calls a method on a *class/type*, but binds to the *instance*.
 *	E.g. looks up a method like Class.method()
 *	but then it binds to the instance:
 *	instance = Class()
 *	instance.method()
 */
static PyObject *
call_method(PyObject *o, char *name, PyObject **nameobj, char *format, ...)
{
	va_list va;
	PyObject *args, *func = 0, *retval;
	va_start(va, format);

	func = _PyObject_LookupSpecial(o, name, nameobj);
	if (func == NULL) {
		va_end(va);
		if (!PyErr_Occurred())
			PyErr_SetObject(PyExc_AttributeError, *nameobj);
		return NULL;
	}

	if (format && *format)
		args = Py_VaBuildValue(format, va);
	else
		args = PyTuple_New(0);

	va_end(va);

	if (args == NULL)
		return NULL;

	assert(PyTuple_Check(args));
	retval = PyObject_Call(func, args, NULL);

	Py_DECREF(args);
	Py_DECREF(func);

	return retval;
}


/**
 *	Copied from typeobject.c.
 */
static int
slot_tp_setattro(PyObject *self, PyObject *name, PyObject *value)
{
	PyObject *res;
	static PyObject *delattr_str, *setattr_str;

	if (value == NULL)
		res = call_method(self, "__delattr__", &delattr_str,
						  "(O)", name);
	else
		res = call_method(self, "__setattr__", &setattr_str,
						  "(OO)", name, value);
	if (res == NULL)
		return -1;
	Py_DECREF(res);
	return 0;
}


/**
 *	Table mapping the __setattr__ name to the tp_setattro offset and
 *	slot_tp_setattro wrapper function.
 *	Based on slotdefs from typeobject.c.
 */
static wrapperbase g_setattrWrappers[] =
{
	{
		"__setattr__", // name of wrapper
		offsetof( PyTypeObject, tp_setattro ), // offset to tp_setattro slot
		slot_tp_setattro, // function
		wrap_setattr, // wrapper for function
		PyDoc_STR( "x.__setattr__('name', value) <==> x.name = value" ), // doc for wrapper
		0, // flags
		nullptr // name_strobj
	},
	{ nullptr } // Sentinel
};


static wrapperbase g_delattrWrappers[] =
{
	{
		"__delattr__", // name of wrapper
		offsetof( PyTypeObject, tp_setattro ), // offset to tp_setattro slot
		slot_tp_setattro, // function
		wrap_delattr, // wrapper for function
		PyDoc_STR( "x.__delattr__('name') <==> del x.name" ), // doc for wrapper
		0, // flags
		nullptr // name_strobj
	},
	{ nullptr } // Sentinel
};


const char * g_hookCountName = "__setattrHookCount";
PyScript::ScriptString g_pyHookCountName;

const char * g_originalSetattrName = "__originalSetattr";
PyScript::ScriptString g_pyOriginalSetattrName;
#endif // ENABLE_PYTHON_LISTENER_HOOKS


namespace ReflectedPython
{


HookListener::HookListener()
	: entered_( 0 )
{
}


void HookListener::preSetValue( const PropertyAccessor & accessor,
	const Variant & value ) /*override*/
{
	++entered_;
}


void HookListener::postSetValue( const PropertyAccessor & accessor,
	const Variant & value ) /*override*/
{
	--entered_;
}


bool HookListener::entered() const
{
	return (entered_ > 0);
}


void getType( PyScript::ScriptObject & pythonObject,
	PyScript::ScriptClass & outClassObject,
	PyScript::ScriptType & outTypeObject,
	PyScript::ScriptDict & outTypeDict )
{
	if (PyScript::ScriptType::check( pythonObject ))
	{
		// Object is already a *type*
		outTypeObject = PyScript::ScriptType::create( pythonObject );
		outTypeDict = outTypeObject.getDict();
	}
	else if (PyScript::ScriptClass::check( pythonObject ))
	{
		// Object is already a *type*
		outClassObject = PyScript::ScriptClass::create( pythonObject );
		outTypeDict = outClassObject.getDict();
	}
	else
	{
		// Get type of object
		outTypeObject = PyScript::ScriptType::getType( pythonObject );
		outTypeDict = outTypeObject.getDict();
	}
	assert( outTypeDict.exists() );
}


void attachListenerHooks( PyScript::ScriptObject & pythonObject )
{
#if ENABLE_PYTHON_LISTENER_HOOKS
	// __setattr__ hooks are added to the Python *type*, not the *instance*
	// So we must count how many reflected Python objects are using the type
	// Add an attribute to the object to track the number of reflected Python objects
	PyScript::ScriptClass classObject;
	PyScript::ScriptType typeObject;
	PyScript::ScriptDict dict;
	getType( pythonObject, classObject, typeObject, dict );
	if (classObject.exists())
	{
		// TODO old-style class type not supported
		return;
	}

	// Do this first time
	if (!g_pyHookCountName.exists())
	{
		g_pyHookCountName = PyScript::ScriptString( PyString_InternFromString( g_hookCountName ),
			PyScript::ScriptObject::FROM_NEW_REFERENCE );
		assert( g_pyHookCountName.exists() && "Out of memory interning listener hook names" );
	}
	// Do this first time
	if (g_pyOriginalSetattrName == nullptr)
	{
		g_pyOriginalSetattrName = PyScript::ScriptString( PyString_InternFromString( g_originalSetattrName ),
			PyScript::ScriptObject::FROM_NEW_REFERENCE );
		assert( g_pyOriginalSetattrName.exists() && "Out of memory interning listener hook names" );
	}

	// Ignore errors if the item does not exist - first time hook is added
	// We use they type's dict rather than typeObject.get/setAttribute()
	// because often types do not permit modification
	// but we need to attach this data somehow
	auto hookCountObject = dict.getItem( g_pyHookCountName,
		PyScript::ScriptErrorClear() );
	if (hookCountObject.exists())
	{
		// Already hooked, increment count
		auto hookCountLong = PyScript::ScriptLong::create( hookCountObject );
		assert( hookCountLong.exists() );
		if (hookCountLong.exists())
		{
			auto hookCount = hookCountLong.asLong( PyScript::ScriptErrorClear() );
			++hookCount;

			hookCountLong = PyScript::ScriptLong::create( hookCount );
			assert( hookCountLong.exists() );
			assert( hookCountLong.asLong( PyScript::ScriptErrorRetain() ) > 0 );

			// Check for overflow after incrementing hookCount
			assert( !PyScript::Script::hasError() );
#if defined( _DEBUG )
			PyScript::Script::clearError();
#endif // defined( _DEBUG )

			const auto setSuccess = dict.setItem( g_pyHookCountName,
				hookCountLong,
				PyScript::ScriptErrorClear() );
			assert( setSuccess );
			if (typeObject.exists())
			{
				typeObject.modified();
			}

			return;
		}
	}

	// Add counter
	const long hookCount = 1;
	auto hookCountLong = PyScript::ScriptLong::create( hookCount );
	assert( hookCountLong.exists() );
	assert( hookCountLong.asLong( PyScript::ScriptErrorClear() ) > 0 );
	const auto setSuccess1 = dict.setItem( g_pyHookCountName,
		hookCountLong,
		PyScript::ScriptErrorClear() );
	assert( setSuccess1 );

	// -- From typeobject.c add_operators
	auto & setattrWrapperBase = g_setattrWrappers[ 0 ];
	if (setattrWrapperBase.name_strobj == nullptr)
	{
		setattrWrapperBase.name_strobj = PyString_InternFromString( setattrWrapperBase.name );
		assert( setattrWrapperBase.name_strobj && "Out of memory interning listener hook names" );
	}

	auto & delattrWrapperBase = g_delattrWrappers[ 0 ];
	if (delattrWrapperBase.name_strobj == nullptr)
	{
		delattrWrapperBase.name_strobj = PyString_InternFromString( delattrWrapperBase.name );
		assert( delattrWrapperBase.name_strobj && "Out of memory interning listener hook names" );
	}

	auto setattrWrapperName = PyScript::ScriptString( setattrWrapperBase.name_strobj,
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE );

	// Attach setattr hook
	auto originalSetattr = dict.getItem( setattrWrapperName,
		PyScript::ScriptErrorClear() );
	if (originalSetattr != nullptr)
	{
		assert( g_pyOriginalSetattrName.exists() );
		const auto setSuccess2 = dict.setItem( g_pyOriginalSetattrName,
			originalSetattr,
			PyScript::ScriptErrorClear() );
		assert( setSuccess2 );
	}

	if (typeObject.exists())
	{
		auto pyType = reinterpret_cast< PyTypeObject * >( typeObject.get() );
		void * pFunctionToBeWrapped = pyType->tp_setattro;

		// setattr
		assert( pyType->tp_setattro != static_cast< setattrofunc >( setattrWrapperBase.function ) );
		auto setattrWrapperObject = PyScript::ScriptDescrWrapper::create( typeObject,
			setattrWrapperBase,
			pFunctionToBeWrapped );
		assert( setattrWrapperObject.exists() );
		const auto setSuccess3 = dict.setItem( setattrWrapperObject.name(),
			setattrWrapperObject,
			PyScript::ScriptErrorClear() );
		assert( setSuccess3 );

		// delattr
		assert( setattrWrapperBase.function == delattrWrapperBase.function );
		auto delattrWrapperObject = PyScript::ScriptDescrWrapper::create( typeObject,
			delattrWrapperBase,
			pFunctionToBeWrapped );
		assert( delattrWrapperObject.exists() );
		const auto setSuccess4 = dict.setItem( delattrWrapperObject.name(),
			delattrWrapperObject,
			PyScript::ScriptErrorClear() );
		assert( setSuccess4 );

		// -- From typeobject.c update_one_slot
		pyType->tp_setattro = static_cast< setattrofunc >( setattrWrapperBase.function );
		assert( pyType->tp_setattro != static_cast< setattrofunc >( pFunctionToBeWrapped ) );

		typeObject.modified();
	}
	else
	{
		assert( classObject.exists() );
		// TODO old-style class type not supported
	}

#endif // ENABLE_PYTHON_LISTENER_HOOKS
}


void detachListenerHooks( PyScript::ScriptObject & pythonObject )
{
#if ENABLE_PYTHON_LISTENER_HOOKS
	if (g_pHookContext == nullptr)
	{
		// Some other system is keeping a Python object after Python has
		// shut down
		return;
	}

	// __setattr__ hooks are added to the Python *type*, not the *instance*
	// So we must count how many reflected Python objects are using the type
	// Add an attribute to the object to track the number of reflected Python objects
	PyScript::ScriptClass classObject;
	PyScript::ScriptType typeObject;
	PyScript::ScriptDict dict;
	getType( pythonObject, classObject, typeObject, dict );
	if (classObject.exists())
	{
		// TODO not supported
		return;
	}

	auto hookCountObject = dict.getItem( g_pyHookCountName,
		PyScript::ScriptErrorClear() );
	if (!hookCountObject.exists())
	{
		// Not hooked
		// An error must have occured in attachListenerHooks()
		return;
	}

	// Decrement count
	auto hookCountLong = PyScript::ScriptLong::create( hookCountObject );
	assert( hookCountLong.exists() );
	if (!hookCountLong.exists())
	{
		return;
	}

	auto hookCount = hookCountLong.asLong( PyScript::ScriptErrorClear() );
	assert( hookCount > 0 );
	--hookCount;

	if (hookCount > 0)
	{
		// Still other reflected Python objects using this type
		// Update new count
		hookCountLong = PyScript::ScriptLong::create( hookCount );
		assert( hookCountLong.exists() );
		assert( hookCountLong.asLong( PyScript::ScriptErrorClear() ) > 0 );
		const auto setSuccess = dict.setItem( g_pyHookCountName,
			hookCountLong,
			PyScript::ScriptErrorClear() );
		assert( setSuccess );

		if (typeObject.exists())
		{
			typeObject.modified();
		}
		return;
	}

	auto & setattrWrapperBase = g_setattrWrappers[ 0 ];
	assert( setattrWrapperBase.name_strobj != nullptr );
	auto setattrWrapperName = PyScript::ScriptString( setattrWrapperBase.name_strobj,
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE );

	auto & delattrWrapperBase = g_delattrWrappers[ 0 ];
	assert( delattrWrapperBase.name_strobj != nullptr );
	auto delattrWrapperName = PyScript::ScriptString( delattrWrapperBase.name_strobj,
		PyScript::ScriptObject::FROM_BORROWED_REFERENCE );

	assert( g_pyOriginalSetattrName.exists() );
	const auto originalSetattr = dict.getItem( g_pyOriginalSetattrName,
		PyScript::ScriptErrorClear() );

	const auto setattrWrapperObject = dict.getItem( setattrWrapperName,
		PyScript::ScriptErrorClear() );

	const auto delattrWrapperObject = dict.getItem( delattrWrapperName,
		PyScript::ScriptErrorClear() );

	if (originalSetattr.exists())
	{
		const auto setSuccess1 = dict.setItem( setattrWrapperName,
			originalSetattr,
			PyScript::ScriptErrorClear() );
		assert( setSuccess1 );

		const auto setSuccess2 = dict.setItem( delattrWrapperName,
			originalSetattr,
			PyScript::ScriptErrorClear() );
		assert( setSuccess2 );

		const auto removeSuccess = dict.delItem( g_pyOriginalSetattrName,
			PyScript::ScriptErrorClear() );
		assert( removeSuccess );
	}
	else
	{
		const auto removeSuccess1 = dict.delItem( setattrWrapperName,
			PyScript::ScriptErrorClear() );
		assert( removeSuccess1 );

		const auto removeSuccess2 = dict.delItem( delattrWrapperName,
			PyScript::ScriptErrorClear() );
		assert( removeSuccess2 );
	}

	assert( g_pyHookCountName.exists() );
	const auto removeSuccess = dict.delItem( g_pyHookCountName,
		PyScript::ScriptErrorClear() );
	assert( removeSuccess );

	// Restore old setattr
	if (typeObject.exists())
	{
		auto pyType = reinterpret_cast< PyTypeObject * >( typeObject.get() );
		auto pyWrapper = reinterpret_cast< PyWrapperDescrObject * >( setattrWrapperObject.get() );
		assert( pyType->tp_setattro == static_cast< setattrofunc >( setattrWrapperBase.function ) );
		assert( pyType->tp_setattro != static_cast< setattrofunc >( pyWrapper->d_wrapped ) );
		pyType->tp_setattro = static_cast< setattrofunc >( pyWrapper->d_wrapped );

		typeObject.modified();
	}
	else
	{
		assert( classObject.exists() );
		// TODO old-style class type not supported
	}

#endif // ENABLE_PYTHON_LISTENER_HOOKS
}


} // namespace ReflectedPython
} // end namespace wgt
