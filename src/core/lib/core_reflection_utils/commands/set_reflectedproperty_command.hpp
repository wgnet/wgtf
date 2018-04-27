#ifndef SET_REFLECTED_PROPERTY_COMMAND_HPP
#define SET_REFLECTED_PROPERTY_COMMAND_HPP

#include "core_command_system/command.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/property_accessor.hpp"

namespace wgt
{
class IDefinitionManager;

class ReflectedPropertyCommandArgument
{
	DECLARE_REFLECTED

public:
	ReflectedPropertyCommandArgument();

	const RefObjectId& getContextId() const;
	const char* getPropertyPath() const;
	const Variant& getPropertyValue() const;

	void setContextId(const RefObjectId& id);
	void setPath(const char* path);
	void setValue(const Variant& value);

private:
	RefObjectId contextId_;
	std::string propertyPath_;
	Variant value_;
};

class SetReflectedPropertyCommand : public Command
{
public:
	SetReflectedPropertyCommand(IDefinitionManager& definitionManager);
	~SetReflectedPropertyCommand() override;

	const char* getId() const override;
	const char* getName() const override;
	virtual Variant execute(const ObjectHandle& arguments) const override;
	bool validateArguments(const ObjectHandle& arguments) const override;
	CommandThreadAffinity threadAffinity() const override;
	ManagedObjectPtr copyArguments(const ObjectHandle& arguments) const override;

private:
	IDefinitionManager& definitionManager_;
};
} // end namespace wgt
#endif // SET_REFLECTED_PROPERTY_COMMAND_HPP
