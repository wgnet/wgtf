#ifndef I_LOGGING_MODEL_HPP
#define I_LOGGING_MODEL_HPP
#pragma once

#include <string>

#include <core_dependency_system/i_interface.hpp>
#include <core_reflection/i_definition_manager.hpp>
#include <core_reflection/property_accessor.hpp>
#include <core_reflection/reflected_object.hpp>

namespace wgt
{
class ILogger;
class IValueChangeNotifier;

class ILoggingModel
{
	DECLARE_REFLECTED

private:

#define SET_REFLECTED_PROPERTY(prop) \
	auto definition = Context::queryInterface<IDefinitionManager>()->getDefinition<ILoggingModel>(); \
	auto handle = ObjectHandleT<ILoggingModel>(this, definition); \
	PropertyAccessor pa = definition->bindProperty(#prop, handle); \
	pa.setValue(prop);
	
public:

	virtual const std::string& getText() const = 0;
	virtual void appendText( const std::string& text ) = 0;
	virtual void clear() = 0;


	void setText( const std::string& text )
	{
		SET_REFLECTED_PROPERTY( text );
	}

	// for reflection ONLY!  do not call from code or the signal to update will not get fired
	virtual void putText( const std::string& text ) = 0;
};
} // end namespace wgt

#endif // I_LOGGING_MODEL_HPP
