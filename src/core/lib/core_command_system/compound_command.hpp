#ifndef COMPOUND_COMMAND_HPP
#define COMPOUND_COMMAND_HPP

#include "command.hpp"
#include "core_reflection/reflected_object.hpp"

namespace wgt
{
class MacroObject;
class IReflectionController;

class CompoundCommand
	: public Command
{
	DECLARE_REFLECTED
public:
	friend CommandManagerImpl;
	typedef std::vector< std::pair<std::string, ObjectHandle> > SubCommandCollection;
	CompoundCommand();
	~CompoundCommand();
	
	const char * getId() const override;
	ObjectHandle execute(const ObjectHandle & arguments) const override;
	bool validateArguments(const ObjectHandle & arguments) const override;
	CommandThreadAffinity threadAffinity() const override;
	void addCommand( const char * commandId, const ObjectHandle & commandArguments );
	ObjectHandle getMacroObject() const;
	const SubCommandCollection & getSubCommands() const;

	void serialize(ISerializer & serializer) const;
	void deserialize(ISerializer & serializer);

private:
	void initDisplayData( IDefinitionManager & defManager, IReflectionController* controller );
	void setId( const char * id );
	
	SubCommandCollection subCommands_;
	std::string id_;
	ObjectHandleT<MacroObject> macroObject_;
};
} // end namespace wgt
#endif //COMPOUND_COMMAND_HPP
