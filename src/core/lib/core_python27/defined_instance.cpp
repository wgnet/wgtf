#include "pch.hpp"

#include "defined_instance.hpp"
#include "definition_details.hpp"

#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "interfaces/core_python_script/i_scripting_engine.hpp"
#include "i_script_object_definition_registry.hpp"
#include "core_dependency_system/wgt_static_init.hpp"
#include "core_dependency_system/depends.hpp"

namespace wgt
{
namespace ReflectedPython
{
DefinedInstance::DefinedInstance()
    : BaseGenericObject(), pythonObject_(nullptr), pDefinition_(nullptr), parentHandle_(nullptr), pRoot_(this)
{
	assert(false && "Always construct with a Python object");
}

DefinedInstance::DefinedInstance(const PyScript::ScriptObject& pythonObject,
                                 std::shared_ptr<IClassDefinition>& definition, const ObjectHandle& parentHandle,
                                 const std::string& childPath)
    : BaseGenericObject(), pythonObject_(pythonObject), pDefinition_(definition), parentHandle_(parentHandle),
      pRoot_(this)
{
	auto pParentInstance = parentHandle_.getBase<DefinedInstance>();

	// parentHandle_ must be null or contain a DefinedInstance
	assert(parentHandle_.isValid() || (pParentInstance == nullptr));
	if (pParentInstance != nullptr)
	{
		// Detect cycles
		assert(pParentInstance->pythonObject() != pythonObject_);

		// Update root object
		pRoot_ = &pParentInstance->root();

		// Add parent path
		fullPath_ = pParentInstance->fullPath();
		if (!fullPath_.empty() && !childPath.empty())
		{
			if (childPath[0] != Collection::getIndexOpen())
			{
				fullPath_ += IClassDefinition::DOT_OPERATOR;
			}
		}
	}

	// Add child path after parent path
	fullPath_ += childPath;

	setDefinition(pDefinition_.get());
}

DefinedInstance::~DefinedInstance()
{
}

const PyScript::ScriptObject& DefinedInstance::pythonObject() const
{
	return pythonObject_;
}

const DefinedInstance& DefinedInstance::root() const
{
	assert(pRoot_ != nullptr);
	return (*pRoot_);
}

const std::string& DefinedInstance::fullPath() const
{
	return fullPath_;
}

ObjectHandle DefinedInstance::getDerivedType() const
{
	auto pRegistry = Depends::get<IScriptObjectDefinitionRegistry>();
	assert(pRegistry != nullptr);
	auto& registry = (*pRegistry);

	auto pObjectManager = Depends::get<IObjectManager>();
	assert(pObjectManager != nullptr);
	auto& objectManager = (*pObjectManager);

	const auto& id = registry.getID(pythonObject_);
	return objectManager.getObject(id);
}

ObjectHandle DefinedInstance::getDerivedType()
{
	auto pRegistry = Depends::get<IScriptObjectDefinitionRegistry>();
	assert(pRegistry != nullptr);
	auto& registry = (*pRegistry);

	auto pObjectManager = Depends::get<IObjectManager>();
	assert(pObjectManager != nullptr);
	auto& objectManager = (*pObjectManager);

	const auto& id = registry.getID(pythonObject_);
	return objectManager.getObject(id);
}

TextStream& operator<<(TextStream& stream, const DefinedInstance& value)
{
	if (auto definition = value.getDefinition())
	{
		stream << definition->getName();
	}
	else
	{
		stream << "<empty DefinedInstance>";
	}

	return stream;
}

} // namespace ReflectedPython

MetaTypeImpl<ReflectedPython::DefinedInstance>::MetaTypeImpl() : base("DefinedInstance", data<value_type>())
{
}

void MetaTypeImpl<ReflectedPython::DefinedInstance>::init(void* value) const
{
	new (value) value_type();
}

void MetaTypeImpl<ReflectedPython::DefinedInstance>::copy(void* dest, const void* src) const
{
	// should never be called
	assert(false);
}

void MetaTypeImpl<ReflectedPython::DefinedInstance>::move(void* dest, void* src) const
{
	// should never be called
	assert(false);
}

void MetaTypeImpl<ReflectedPython::DefinedInstance>::destroy(void* value) const
{
	cast(value).~value_type();
}

bool MetaTypeImpl<ReflectedPython::DefinedInstance>::lessThan(const void* lhs, const void* rhs) const
{
	// different instances are always different
	return lhs < rhs;
}

bool MetaTypeImpl<ReflectedPython::DefinedInstance>::equal(const void* lhs, const void* rhs) const
{
	// different instances are always different
	return lhs == rhs;
}

void MetaTypeImpl<ReflectedPython::DefinedInstance>::streamOut(TextStream& stream, const void* value) const
{
	stream << cast(value);
}

void MetaTypeImpl<ReflectedPython::DefinedInstance>::streamIn(TextStream& stream, void* value) const
{
	stream.setState(std::ios_base::failbit);
}

void MetaTypeImpl<ReflectedPython::DefinedInstance>::streamOut(BinaryStream& stream, const void* value) const
{
	stream.setState(std::ios_base::failbit);
}

void MetaTypeImpl<ReflectedPython::DefinedInstance>::streamIn(BinaryStream& stream, void* value) const
{
	stream.setState(std::ios_base::failbit);
}

} // end namespace wgt
