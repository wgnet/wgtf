#ifndef SET_REFLECTED_PROPERTY_COMMAND_HPP
#define SET_REFLECTED_PROPERTY_COMMAND_HPP

#include "core_command_system/command.hpp"
#include "core_reflection/reflected_object.hpp"

namespace wgt
{
class IDefinitionManager;

class ReflectedPropertyCommandArgument
{
	DECLARE_REFLECTED

public:
	ReflectedPropertyCommandArgument();

	const RefObjectId & getContextId() const;
	const char * getPropertyPath() const;
	const Variant & getPropertyValue() const;

	void setContextId( const RefObjectId & id );
	void setPath( const char * path );
	void setValue( const Variant & value );

	static const char * contextIdPropertyName();
	static const char * pathPropertyName();
	static const char * valuePropertyName();
    
private:
	RefObjectId contextId_;
	std::string propertyPath_;
	Variant		value_;

	static const char * s_ContextId;
	static const char * s_PropertyPath;
	static const char * s_PropertyValue;
};

class SetReflectedPropertyCommand
	: public Command
{
public:
	SetReflectedPropertyCommand( IDefinitionManager & definitionManager );
	~SetReflectedPropertyCommand() override;

	const char * getId() const override;
	ObjectHandle execute(const ObjectHandle & arguments ) const override;
	bool validateArguments(const ObjectHandle& arguments) const override;
	CommandThreadAffinity threadAffinity() const override;

private:
	IDefinitionManager & definitionManager_;
};
} // end namespace wgt
#endif //SET_REFLECTED_PROPERTY_COMMAND_HPP
