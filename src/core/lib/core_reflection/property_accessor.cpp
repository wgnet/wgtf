#include "property_accessor.hpp"
#include "interfaces/i_base_property.hpp"
#include "reflected_object.hpp"
#include "reflected_method.hpp"
#include "reflected_method_parameters.hpp"
#include "object_handle.hpp"
#include "property_accessor_listener.hpp"
#include "i_definition_manager.hpp"
#include "utilities/reflection_utilities.hpp"
#include "metadata/meta_base.hpp"

#include <unordered_map>
#include "core_variant/variant.hpp"

namespace wgt
{
//==============================================================================
PropertyAccessor::PropertyAccessor( PropertyAccessor && other )
	: parentAccessor_( std::move(other.parentAccessor_) )
    , object_( other.object_ )
    , property_( std::move( other.property_ ) )
    , rootObject_( other.rootObject_ )
    , path_( std::move( other.path_ ) )
    , definitionManager_( other.definitionManager_ )
{
}

//==============================================================================
PropertyAccessor::PropertyAccessor( const PropertyAccessor & other )
    : parentAccessor_( other.parentAccessor_ )
    , object_( other.object_ )
    , property_( other.property_ )
    , rootObject_( other.rootObject_ )
    , path_( other.path_ )
    , definitionManager_( other.definitionManager_ )
{
}

//==============================================================================
PropertyAccessor::PropertyAccessor()
	: property_( nullptr )
	, definitionManager_( nullptr )
{
}

//==============================================================================
PropertyAccessor& PropertyAccessor::operator = (const PropertyAccessor & other)
{
	if (this == &other)
	{
		return *this;
	}

	object_ = other.object_;
	property_ = other.property_;
	rootObject_ = other.rootObject_;
	path_ = other.path_;
	definitionManager_ = other.definitionManager_;
	parentAccessor_ = other.parentAccessor_;
	return *this;
}

//==============================================================================
PropertyAccessor& PropertyAccessor::operator = (PropertyAccessor&& other)
{
	object_ = other.object_;
	property_ = std::move( other.property_ );
	rootObject_ = other.rootObject_;
	path_ = std::move( other.path_ );
	definitionManager_ = other.definitionManager_;
	parentAccessor_ = other.parentAccessor_;
	return *this;
}


//==============================================================================
PropertyAccessor::PropertyAccessor(
	const IDefinitionManager * definitionManager,
	const ObjectHandle & rootObject, const char * path )
	:property_( nullptr )
	, rootObject_( rootObject )
	, path_( path )
	, definitionManager_( definitionManager )
{
}


//==============================================================================
bool PropertyAccessor::isValid() const
{
	return object_ != ObjectHandle() && getProperty() != nullptr;
}


//==============================================================================
const TypeId & PropertyAccessor::getType() const
{
	if (!isValid())
	{
		static auto s_voidType = TypeId::getType< void >();
		return s_voidType;
	}
	return getProperty()->getType();
}


//==============================================================================
PropertyAccessor PropertyAccessor::getParent() const
{
	if (path_.length() == 0||
		definitionManager_ == nullptr)
	{
		return PropertyAccessor();
	}
	auto definition = rootObject_.getDefinition( *definitionManager_ );
	if (definition == nullptr)
	{
		return PropertyAccessor();
	}
	// This is the member operator '.', the arrow operator '->', and array operator '['
	size_t parentMarkerPos = path_.find_last_of( ".[" );
	if (parentMarkerPos == std::string::npos)
	{
		return definition->bindProperty( "", rootObject_ );
	}
	std::string parentPath;
	parentPath.append( path_.c_str(), parentMarkerPos );
	return definition->bindProperty( parentPath.c_str(), rootObject_ );
}


//==============================================================================
bool PropertyAccessor::canSetValue() const
{
	if (!this->isValid())
	{
		return false;
	}

	if (definitionManager_ == nullptr)
	{
		return false;
	}

	return !this->getProperty()->readOnly();
}


//==============================================================================
bool PropertyAccessor::setValue( const Variant & value ) const 
{
	if (!this->canSetValue())
	{
		return false;
	}

	// Since "listeners" is a MutableVector, these iterators are safe to use
	// while other listeners are registered/deregistered
	auto& listeners = definitionManager_->getPropertyAccessorListeners();
	auto itBegin = listeners.cbegin();
	auto itEnd = listeners.cend();
	for( auto it = itBegin; it != itEnd; ++it )
	{
		auto listener = it->lock();
		assert( listener != nullptr );
		listener->preSetValue( *this, value );
	}
	bool ret = getProperty()->set( object_, value, *definitionManager_ );
	// Set the parent object to support properties returned by value
	if( parentAccessor_ )
	{
		parentAccessor_->setValue( object_ );
	}
	for( auto it = itBegin; it != itEnd; ++it )
	{
		auto listener = it->lock();
		assert( listener != nullptr );
		listener->postSetValue( *this, value );
	}
	return ret;
}


//==============================================================================
bool PropertyAccessor::setValueWithoutNotification( const Variant & value ) const 
{
	if (!isValid())
	{
		return false;
	}
	if (definitionManager_ == nullptr)
	{
		return false;
	}

	return getProperty()->set( object_, value, *definitionManager_ );
}


//==============================================================================
bool PropertyAccessor::canInvoke() const
{
	if (!this->isValid())
	{
		return false;
	}

	if (definitionManager_ == nullptr)
	{
		return false;
	}

	return this->getProperty()->isMethod();
}


//==============================================================================
Variant PropertyAccessor::invoke( const ReflectedMethodParameters & parameters ) const
{
	Variant result;

	if (!this->canInvoke())
	{
		return result;
	}

	const auto& listeners = definitionManager_->getPropertyAccessorListeners();

	for (auto itr = listeners.cbegin(); itr != listeners.cend(); ++itr)
	{
		auto listener = itr->lock();
		assert( listener != nullptr );
		listener->preInvoke( *this, parameters, false );
	}

	result = getProperty()->invoke( object_, *definitionManager_, parameters );

	for (auto itr = listeners.cbegin(); itr != listeners.cend(); ++itr)
	{
		auto listener = itr->lock();
		assert( listener != nullptr );
		listener->postInvoke( *this, result, false );
	}

	return result;
}


//==============================================================================
void PropertyAccessor::invokeUndoRedo( const ReflectedMethodParameters & parameters, Variant result, bool undo ) const
{
	if (!this->canInvoke())
	{
		return;
	}

	const auto& listeners = definitionManager_->getPropertyAccessorListeners();

	for (auto itr = listeners.cbegin(); itr != listeners.cend(); ++itr)
	{
		auto listener = itr->lock();
		// What does this assertion mean? Isn't this to be expected sometimes?
		// I encountered this assertion during shutdown. @m_martin
		assert( listener != nullptr );
		listener->preInvoke( *this, parameters, undo );
	}

	ReflectedMethod* method = static_cast<ReflectedMethod*>( getProperty().get() );
	method = undo ? method->getUndoMethod() : method->getRedoMethod();
	assert( method != nullptr );
	ReflectedMethodParameters paramsUndoRedo;
	paramsUndoRedo.push_back( ObjectHandle(parameters) );
	paramsUndoRedo.push_back( result );
	method->invoke( object_, *definitionManager_, paramsUndoRedo );

	for (auto itr = listeners.cbegin(); itr != listeners.cend(); ++itr)
	{
		auto listener = itr->lock();
		assert( listener != nullptr );
		listener->postInvoke( *this, result, undo );
	}
}


//==============================================================================
bool PropertyAccessor::canInsert() const
{
	Collection collection;
	auto thisValue = getValue();
	if (!thisValue.tryCast( collection ))
	{
		return false;
	}

	return collection.canResize();
}


//==============================================================================
bool PropertyAccessor::insert( const Variant & key, const Variant & value ) const
{
	Collection collection;
	auto thisValue = getValue();
	if (!thisValue.tryCast( collection ))
	{
		return false;
	}

	if (!collection.canResize())
	{
		return false;
	}

	// Since "listeners" is a MutableVector, these iterators are safe to use
	// while other listeners are registered/deregistered
	auto& listeners = definitionManager_->getPropertyAccessorListeners();
	auto itBegin = listeners.cbegin();
	auto itEnd = listeners.cend();

	auto preInsert = collection.connectPreInsert( [&]( Collection::Iterator pos, size_t count)
	{
		auto index = std::distance( collection.begin(), pos );
		for( auto it = itBegin; it != itEnd; ++it )
		{
			auto listener = it->lock();
			assert( listener != nullptr );
			listener->preInsert( *this, index, count );
		}
	} );
	auto postInserted = collection.connectPostInserted( [&]( Collection::Iterator pos, size_t count)
	{
		auto index = std::distance( collection.begin(), pos );
		for( auto it = itBegin; it != itEnd; ++it )
		{
			auto listener = it->lock();
			assert( listener != nullptr );
			listener->postInserted( *this, index, count );
		}
	} );

	auto it = collection.insert( key );
	it.setValue( value );

	preInsert.disconnect();
	postInserted.disconnect();
	
	return it != collection.end();
}


//==============================================================================
bool PropertyAccessor::canErase() const
{
	Collection collection;
	auto thisValue = getValue();
	if (!thisValue.tryCast( collection ))
	{
		return false;
	}

	return collection.canResize();
}


//==============================================================================
bool PropertyAccessor::erase( const Variant & key ) const
{
	Collection collection;
	auto thisValue = getValue();
	if (!thisValue.tryCast( collection ))
	{
		return false;
	}

	if (!collection.canResize())
	{
		return false;
	}

	// Since "listeners" is a MutableVector, these iterators are safe to use
	// while other listeners are registered/deregistered
	auto& listeners = definitionManager_->getPropertyAccessorListeners();
	auto itBegin = listeners.cbegin();
	auto itEnd = listeners.cend();

	auto preErase = collection.connectPreErase( [&]( Collection::Iterator pos, size_t count)
	{
		auto index = std::distance( collection.begin(), pos );
		for( auto it = itBegin; it != itEnd; ++it )
		{
			auto listener = it->lock();
			assert( listener != nullptr );
			listener->preErase( *this, index, count );
		}
	} );
	auto postErased = collection.connectPostErased( [&]( Collection::Iterator pos, size_t count)
	{
		auto index = std::distance( collection.begin(), pos );
		for( auto it = itBegin; it != itEnd; ++it )
		{
			auto listener = it->lock();
			assert( listener != nullptr );
			listener->postErased( *this, index, count );
		}
	} );

	auto count = collection.eraseKey( key );

	preErase.disconnect();
	postErased.disconnect();

	return count > 0;
}


//==============================================================================
const char * PropertyAccessor::getName() const
{
	if(!isValid())
	{
		return nullptr;
	}
	return getProperty()->getName();
}


//==============================================================================
const IClassDefinition * PropertyAccessor::getStructDefinition() const
{
	if(!isValid())
	{
		return nullptr;
	}
	if (definitionManager_ == nullptr)
	{
		return nullptr;
	}
	Collection collection;
	bool isCollection = getValue().tryCast( collection );
	if (isCollection)
	{
		return 
			definitionManager_->getDefinition(
			collection.valueType().getName()
		);
	}
	return definitionManager_->getDefinition(
		getProperty()->getType().getName() );
}


bool PropertyAccessor::canGetValue() const
{
	if (!this->isValid())
	{
		return false;
	}

	if (definitionManager_ == nullptr)
	{
		return false;
	}

	return this->getProperty()->isValue();
}


//==============================================================================
Variant PropertyAccessor::getValue() const
{
	// TODO NGT-1649 How does it return a value when it's invalid?
	// Does not match behaviour of setValue()
	if (!this->isValid())
	{
		return this->getRootObject();
	}
	if (definitionManager_ == nullptr)
	{
		return this->getRootObject();
	}

	assert( this->canGetValue() );
	return getProperty()->get( object_, *definitionManager_ );
}


//==============================================================================
MetaHandle PropertyAccessor::getMetaData() const
{
	if (getProperty() == nullptr)
	{
		return nullptr;
	}
	return getProperty()->getMetaData();
}


//==============================================================================
void PropertyAccessor::setObject( const ObjectHandle & object )
{
	object_ = object;
}


//==============================================================================
void PropertyAccessor::setBaseProperty( const IBasePropertyPtr & property )
{
	property_ = property;
}

void PropertyAccessor::setParent(const PropertyAccessor& parent)
{
	parentAccessor_ = std::make_shared<PropertyAccessor>(parent);
}

//==============================================================================
const ObjectHandle & PropertyAccessor::getRootObject() const
{
	return rootObject_;
}


//==============================================================================
const char * PropertyAccessor::getFullPath() const
{
	return path_.c_str();
}


//==============================================================================
const IDefinitionManager * PropertyAccessor::getDefinitionManager() const
{
	return definitionManager_;
}
} // end namespace wgt
