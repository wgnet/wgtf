#ifndef I_LOGGING_MODEL_HPP
#define I_LOGGING_MODEL_HPP
#pragma once

#include "core_dependency_system/i_interface.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_object/object_handle_provider.hpp"

#include <string>

namespace wgt
{
class ILogger;

class ILoggingModel : Depends<IDefinitionManager>
                    , public ObjectHandleProvider<ILoggingModel>
{
	DECLARE_REFLECTED

private:
    // TODO: Fix this, we shouldn't be depending on exposed names in the header
    template<typename T>
    void setReflectedProperty(const char* name, T& value)
    {
        auto defManager = get<IDefinitionManager>();
        auto definition = defManager->getDefinition<ILoggingModel>();
        PropertyAccessor pa = definition->bindProperty(name, handle());
        pa.setValue(value);
    }

	// for reflection ONLY!  do not call from code or the signal to update will not get fired
	virtual void putText(const std::string& text) = 0;

public:
	virtual const std::string& getText() const = 0;
	virtual void appendText(const std::string& text) = 0;
	virtual void clear() = 0;

	virtual void setText(const std::string& text)
	{
        setReflectedProperty("text", text);
	}
};
} // end namespace wgt

#endif // I_LOGGING_MODEL_HPP
