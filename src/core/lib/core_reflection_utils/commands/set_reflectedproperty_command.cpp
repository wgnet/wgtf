#include "set_reflectedproperty_command.hpp"

#include "core_variant/variant.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/utilities/reflection_utilities.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_reflection_utils/commands/reflectedproperty_undoredo_helper.hpp"

namespace wgt
{
//==============================================================================
const char* ReflectedPropertyCommandArgument::s_ContextId = "PropertyContextId";
const char* ReflectedPropertyCommandArgument::s_PropertyPath = "PropertyPath";
const char* ReflectedPropertyCommandArgument::s_PropertyValue = "PropertyValue";

//==============================================================================
const char* ReflectedPropertyCommandArgument::contextIdPropertyName()
{
	return s_ContextId;
}

//==============================================================================
const char* ReflectedPropertyCommandArgument::pathPropertyName()
{
	return s_PropertyPath;
}

//==============================================================================
const char* ReflectedPropertyCommandArgument::valuePropertyName()
{
	return s_PropertyValue;
}

//==============================================================================
ReflectedPropertyCommandArgument::ReflectedPropertyCommandArgument() : contextId_(""), propertyPath_("")
{
}

//==============================================================================
const RefObjectId& ReflectedPropertyCommandArgument::getContextId() const
{
	return contextId_;
}

//==============================================================================
const char* ReflectedPropertyCommandArgument::getPropertyPath() const
{
	return propertyPath_.c_str();
}

//==============================================================================
const Variant& ReflectedPropertyCommandArgument::getPropertyValue() const
{
	return value_;
}

//==============================================================================
void ReflectedPropertyCommandArgument::setContextId(const RefObjectId& id)
{
	contextId_ = id;
}

//==============================================================================
void ReflectedPropertyCommandArgument::setPath(const char* path)
{
	propertyPath_ = path;
}

//==============================================================================
void ReflectedPropertyCommandArgument::setValue(const Variant& value)
{
	value_ = value;
}

//==============================================================================
SetReflectedPropertyCommand::SetReflectedPropertyCommand(IDefinitionManager& definitionManager)
    : definitionManager_(definitionManager)
{
}

//==============================================================================
SetReflectedPropertyCommand::~SetReflectedPropertyCommand()
{
}

//==============================================================================
const char* SetReflectedPropertyCommand::getId() const
{
	static const char* s_Id = getClassIdentifier<SetReflectedPropertyCommand>();
	return s_Id;
}

//==============================================================================
bool SetReflectedPropertyCommand::validateArguments(const ObjectHandle& arguments) const
{
	if (!arguments.isValid())
	{
		return false;
	}

	auto commandArgs = arguments.getBase<ReflectedPropertyCommandArgument>();

	if (commandArgs == nullptr)
	{
		return false;
	}

	auto objManager = definitionManager_.getObjectManager();
	if (objManager == nullptr)
	{
		return false;
	}

	const ObjectHandle& object = objManager->getObject(commandArgs->getContextId());
	if (!object.isValid())
	{
		return false;
	}

	const IClassDefinition* defn = object.getDefinition(definitionManager_);
	PropertyAccessor property = defn->bindProperty(commandArgs->getPropertyPath(), object);
	if (property.isValid() == false)
	{
		return false;
	}

	const MetaType* dataType = commandArgs->getPropertyValue().type();
	const MetaType* propertyValueType = property.getValue().type();

	if (!dataType->canConvertTo(propertyValueType))
	{
		return false;
	}

	return true;
}

//==============================================================================
ObjectHandle SetReflectedPropertyCommand::execute(const ObjectHandle& arguments) const
{
	ReflectedPropertyCommandArgument* commandArgs = arguments.getBase<ReflectedPropertyCommandArgument>();
	auto objManager = definitionManager_.getObjectManager();
	assert(objManager != nullptr);
	ObjectHandle object = objManager->getObject(commandArgs->getContextId());
	if (!object.isValid())
	{
		return CommandErrorCode::INVALID_ARGUMENTS;
	}
	PropertyAccessor property =
	object.getDefinition(definitionManager_)->bindProperty(commandArgs->getPropertyPath(), object);
	if (property.isValid() == false)
	{
		// Can't set
		return CommandErrorCode::INVALID_ARGUMENTS;
	}
	const Variant& data = commandArgs->getPropertyValue();
	bool br = property.setValue(data);
	if (!br)
	{
		return CommandErrorCode::INVALID_VALUE;
	}

	// Do not return the object
	// CommandInstance will hold a reference to the return value
	// and the CommandInstance is stored in the undo/redo history forever
	// This is due to a circular reference in CommandManagerImpl::pushFrame
	return nullptr;
}

//==============================================================================
CommandThreadAffinity SetReflectedPropertyCommand::threadAffinity() const
{
	return CommandThreadAffinity::UI_THREAD;
}
} // end namespace wgt
