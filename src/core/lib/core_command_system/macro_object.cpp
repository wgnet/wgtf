#include "macro_object.hpp"
#include "core_command_system/i_command_manager.hpp"
#include <cassert>
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_command_system/compound_command.hpp"
#include "core_data_model/variant_list.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_string_utils/string_utils.hpp"
#include <codecvt>
#include "core_reflection_utils/commands/set_reflectedproperty_command.hpp"
#include "core_reflection_utils/commands/invoke_reflected_method_command.hpp"
#include "core_reflection_utils/commands/reflectedproperty_undoredo_helper.hpp"
#include "core_data_model/reflection/reflected_tree_model.hpp"
#include "core_variant/type_id.hpp"
#include <sstream>
#include <iostream>
#include <codecvt>

namespace wgt
{
namespace RPURU = ReflectedPropertyUndoRedoUtility;

void MacroEditObject::init( size_t count )
{
	args_.resize( count );
	controllers_.resize( count );
}

const ObjectHandle& MacroEditObject::getCommandArgument(size_t id) const
{
	assert( id < args_.size() );
	return args_[ id ];
}

const ObjectHandle& MacroEditObject::getCommandArgController(size_t id) const
{
	assert( id < controllers_.size() );
	return controllers_[ id ];
}

void MacroEditObject::setCommandHandlers(size_t id, const ObjectHandle & controller, const ObjectHandle & arg)
{
	assert( id < args_.size() );
	controllers_[ id ] = controller;
	args_[ id ] = arg;
}

void MacroEditObject::resolveDependecy(size_t command, const std::vector<CommandInstance*>& instances)
{
	assert( command >= 0 && command < controllers_.size() );
	if (controllers_[ command ].type() == TypeId::getType<ReflectedPropertyCommandArgumentController>())
	{
		auto rpca = controllers_[ command ].getBase<ReflectedPropertyCommandArgumentController>();
		rpca->resolve( instances );
	}
}

MacroObject::MacroObject()
	: commandSystem_( nullptr )
	, pDefManager_( nullptr )
	, cmdId_( "" )
	, macroName_( "" )
{
}

void MacroObject::init( ICommandManager& commandSystem, IDefinitionManager& defManager,
											IReflectionController* controller, const char * cmdId )
{
	commandSystem_ = &commandSystem;
	pDefManager_ = &defManager;
	controller_ = controller;
	cmdId_ = cmdId;
	macroName_ = cmdId_;

	assert(!argsEdit_.isValid());
	bindMacroArgumenets();

}

ObjectHandle MacroObject::executeMacro() const
{
	assert( commandSystem_ != nullptr );
	assert( argsEdit_.isValid() );
	CommandInstancePtr ins = commandSystem_->queueCommand( cmdId_.c_str(), argsEdit_ );
	return ins;
}

const ITreeModel* MacroObject::getTreeModel() const
{
	treeModel_.reset(new ReflectedTreeModel(argsEdit_, *pDefManager_, controller_));
	return treeModel_.get();
}

std::pair<ObjectHandle, ObjectHandle> MacroObject::bind( size_t idx, ReflectedPropertyCommandArgument* rpca ) const
{
	auto argObj = pDefManager_->create<ReflectedPropertyCommandArgument>();
	argObj->setPath( rpca->getPropertyPath() );
	argObj->setValue( rpca->getPropertyValue() );
	argObj->setContextId( rpca->getContextId() );

	auto ctrlObj = createController(idx, argObj);

	return std::pair<ObjectHandle, ObjectHandle>(ctrlObj, argObj);
}

std::pair<ObjectHandle, ObjectHandle> MacroObject::bind( size_t idx, ReflectedMethodCommandParameters* rmcp ) const
{
	auto argObj = pDefManager_->create<ReflectedMethodCommandParameters>();
	argObj->setPath( rmcp->getPath() );
	argObj->setId( rmcp->getId() );
	argObj->setParameters( rmcp->getParameters() );

	auto ctrlObj = createController(idx, argObj);

	return std::pair<ObjectHandle, ObjectHandle>(ctrlObj, argObj);
}

void MacroObject::bindMacroArgumenets()
{
	auto argDef = pDefManager_->getDefinition<MacroEditObject>();
	assert( argDef != nullptr );
	ObjectHandle args = argDef->create();
	MacroEditObject* ccArgs = args.getBase< MacroEditObject >();

	assert( commandSystem_ != nullptr );
	CompoundCommand * macro = static_cast<CompoundCommand *>(commandSystem_->findCommand( cmdId_.c_str() ));
	assert (macro != nullptr);

	const auto& commands = macro->getSubCommands();

	ccArgs->init( commands.size() );

	for (size_t i = 0; i < commands.size(); ++i)
	{
		if (commands[i].second.type() == TypeId::getType<ReflectedPropertyCommandArgument>())
		{
			ReflectedPropertyCommandArgument* rpca = commands[i].second.getBase<ReflectedPropertyCommandArgument>();
			auto p = bind( i, rpca );
			ccArgs->setCommandHandlers( i, p.first, p.second );
		}
		else if (commands[i].second.type() == TypeId::getType<ReflectedMethodCommandParameters>())
		{
			ReflectedMethodCommandParameters* rmcp = commands[i].second.getBase<ReflectedMethodCommandParameters>();
			auto p = bind( i, rmcp );
			ccArgs->setCommandHandlers( i, p.first, p.second );
		}
		else
		{
			ccArgs->setCommandHandlers( i, commands[i].second, commands[i].second );
		}
	}
	
	setArgumentObject(args);
}

void MacroObject::serialize(ISerializer & serializer) const
{
	serializer.serialize( argsEdit_ );
}

void MacroObject::deserialize(ISerializer & serializer)
{
	Variant v;
	serializer.deserialize( v );
	if (v.canCast<ObjectHandle>())
	{
		argsEdit_ = v.cast<ObjectHandle>();
	}
}

ObjectHandle MacroObject::executeMacro(const ObjectHandle& contextObject ) const
{
	assert( commandSystem_ != nullptr );
	assert( argsEdit_.isValid() );

	CommandInstancePtr result = nullptr;

	if ( validateArgsObject(contextObject) )
	{
		result = commandSystem_->queueCommand( cmdId_.c_str(), contextObject );
	}

	return result;
}


bool MacroObject::validateArgsObject(const ObjectHandle & obj) const
{
	Command* command = commandSystem_->findCommand( cmdId_.c_str() );

	if ( command->validateArguments(obj) )
	{
		return true;
	}

	return false;
}

bool MacroObject::setArgumentObject(const ObjectHandle& args)
{
	if ( validateArgsObject(args) )
	{
		argsEdit_ = args;
		return true;
	}

	return false;
}

ObjectHandle MacroObject::createController(size_t idx, const ObjectHandle & args) const
{
	if (args.type() == TypeId::getType<ReflectedPropertyCommandArgument>())
	{
		auto ctrlObj = pDefManager_->create<ReflectedPropertyCommandArgumentController>();
		ctrlObj->init( idx, args, pDefManager_);
		return ctrlObj;
	}
	else if (args.type() == TypeId::getType<ReflectedMethodCommandParameters>())
	{
		auto ctrlObj = pDefManager_->create<ReflectedMethodCommandParametersController>();
		ctrlObj->init( args, pDefManager_);
		return ctrlObj;
	}
	else
	{
		return args;
	}
}

bool MacroObject::setArgumentObjectForCommand(size_t idx, const ObjectHandle& args)
{
	Command* command = commandSystem_->findCommand( cmdId_.c_str() );
	if ( !command ) 
	{
		return false;
	}

	CompoundCommand* compoundCommand = dynamic_cast<CompoundCommand*>(command);
	if (!compoundCommand) 
	{
		return false;
	}

	auto subCommands = compoundCommand->getSubCommands();
	if ( subCommands.size() <= idx ) 
	{
		return false;
	}

	Command* subCommand = commandSystem_->findCommand(subCommands[idx].first.c_str());
	if (!subCommand ) return false;
	if ( !subCommand->validateArguments(args) ) 
	{
		return false;
	}

	MacroEditObject* ccArgs = argsEdit_.getBase<MacroEditObject>();
	if ( !ccArgs ) return false;
	if ( ccArgs->getArgCount() <= idx ) 
	{
		return false;
	}

	ccArgs->setCommandHandlers(idx, createController(idx, args), args);

	return true;
}

ReflectedPropertyCommandArgumentController::ReflectedPropertyCommandArgumentController()
	: arguments_( nullptr )
    , defMngr_( nullptr )
	, subCommandIdx_( 0 )
	, dependencyOffset_( 0 )
{
}

void ReflectedPropertyCommandArgumentController::init(size_t subCommandIdx, ObjectHandle arguments, IDefinitionManager* defMngr)
{
	assert( !arguments_.isValid() );
	assert( arguments.isValid() );
	subCommandIdx_ = subCommandIdx;
	arguments_ = arguments;
	defMngr_ = defMngr;
}

ReflectedPropertyCommandArgument* ReflectedPropertyCommandArgumentController::getArgumentObj() const
{
	assert( arguments_.isValid() && defMngr_ );
	auto arg = arguments_.getBase<ReflectedPropertyCommandArgument>();
	assert( arg );
	return arg;
}

void ReflectedPropertyCommandArgumentController::setValue(const std::string& value)
{
	std::istringstream stream( value );
	Variant v = getArgumentObj()->getPropertyValue();
	stream >> v;
	getArgumentObj()->setValue( v );
}

std::string ReflectedPropertyCommandArgumentController::getValue() const
{
	const Variant& v = getArgumentObj()->getPropertyValue();
	std::ostringstream stream;
	stream << v;
	std::string str = stream.str();
	return str;
}

void ReflectedPropertyCommandArgumentController::setPropertyPath(const std::string& value)
{
	getArgumentObj()->setPath( value.c_str() );
}

const char* ReflectedPropertyCommandArgumentController::getPropertyPath() const
{
	return getArgumentObj()->getPropertyPath();
}

void ReflectedPropertyCommandArgumentController::getObject(int * o_EnumValue) const
{
	if (dependencyOffset_ == 0)
	{
		RefObjectId contextId = getArgumentObj()->getContextId();
		auto it = std::find_if( enumMap_.begin(), enumMap_.end(),
			[&]( const EnumMap::value_type& v ){ return v.first.find( contextId.toWString() ) != std::wstring::npos; } );

		if (it != enumMap_.end())
		{
			*o_EnumValue = (int)std::distance( enumMap_.begin(), it);
		}
	}
	else
	{
		*o_EnumValue = dependencyOffset_;
	}
}
 
void ReflectedPropertyCommandArgumentController::setObject(const int & o_EnumValue)
{
	if (o_EnumValue >= 0)
	{
		auto it = enumMap_.begin();
		std::advance( it, o_EnumValue );
		getArgumentObj()->setContextId( it->second );
		dependencyOffset_ = 0;
	}
	else
	{
		getArgumentObj()->setContextId( RefObjectId() );
		dependencyOffset_ = o_EnumValue;
	}
}

void ReflectedPropertyCommandArgumentController::resolve( const std::vector<CommandInstance*>& instances )
{
	if (dependencyOffset_ == 0)
	{
		return;
	}

	const size_t id = subCommandIdx_ + dependencyOffset_;
	assert( id <= instances.size() );
	assert( instances[id]->getExecutionStatus() == ExecutionStatus::Complete );
	Variant* rv = instances[id]->getReturnValue().getBase<Variant>();
	if (rv && rv->canCast<ObjectHandle>())
	{
		ObjectHandle obj = rv->cast<ObjectHandle>();
		RefObjectId objId;
		if (obj.getId( objId ))
		{
			getArgumentObj()->setContextId( objId );
			return;
		}
	}
	getArgumentObj()->setContextId( RefObjectId() );
}

void filterObjects( const std::string &path, IDefinitionManager* defMngr, EnumMap& enumMap)
{
	IObjectManager* objMngr = defMngr->getObjectManager();
	std::vector< ObjectHandle > objs;
	objMngr->getObjects( objs );

	for (auto & obj : objs)
	{
		const IClassDefinition* def = obj.getDefinition( *defMngr );
		if (def == nullptr)
		{
			continue;
		}

		PropertyAccessor pa;
		ReflectedPropertyUndoRedoUtility::resolveProperty( obj, *def, path.c_str(), pa, *defMngr );
		if (!pa.isValid())
		{
			continue;
		}

		const char* fullPath = pa.getFullPath();
		if (!fullPath || *fullPath == 0)
		{
			continue;
		}

		const char* name = obj.type().getName();

		PropertyAccessor paName;
		ReflectedPropertyUndoRedoUtility::resolveProperty( obj, *def, "name", paName, *defMngr );
		std::string nameProp;
		if (paName.isValid() && paName.getValue().tryCast( nameProp ))
		{
			name = nameProp.c_str();
		}

		assert(name != nullptr && *name != 0);

		RefObjectId id;
		obj.getId( id );

		std::wstring_convert< std::codecvt_utf8< wchar_t > > conv;
		std::wstring wstr = conv.from_bytes( name );
		wstr += L" ";
		wstr += id.toWString();

		enumMap.insert( std::pair<std::wstring, RefObjectId>( wstr, id ) );
	}
}

void ReflectedPropertyCommandArgumentController::generateObjList(std::map< int, std::wstring > * o_enumMap) const
{
	enumMap_.clear();
	o_enumMap->clear();

	std::string path = getPropertyPath();

	for (size_t i = 0; i < subCommandIdx_; ++i)
	{
		o_enumMap->insert( std::pair<int, std::wstring>( static_cast< int >( i - subCommandIdx_ ), std::wstring(L"command ") + std::to_wstring(i) ) );
	}

	filterObjects( path, defMngr_, enumMap_ );
	for (auto it = enumMap_.begin(); it != enumMap_.end(); ++it)
	{
		o_enumMap->insert( std::pair<int, std::wstring>( static_cast< int >( std::distance( enumMap_.begin(), it ) ), it->first) );
	}
}

ReflectedMethodCommandParametersController::ReflectedMethodCommandParametersController()
	: paramsObj_( nullptr )
    , defMngr_( nullptr )
{

}

ReflectedMethodCommandParameters* ReflectedMethodCommandParametersController::getParamObj() const
{
	assert( paramsObj_.isValid() && defMngr_ );
	auto params = paramsObj_.getBase<ReflectedMethodCommandParameters>();
	assert( params );
	return params;
}

void ReflectedMethodCommandParametersController::init(ObjectHandle parameters, IDefinitionManager* defMngr)
{
	assert( !paramsObj_.isValid() );
	assert( parameters.isValid() );
	paramsObj_ = parameters;
	defMngr_ = defMngr;

	ReflectedMethodParameters& refParams = getParamObj()->getParametersRef();
	methodParams_.reserve( refParams.size() );
	for (auto& p : refParams)
	{
		MethodParam mp;
		mp.init( &p );
		methodParams_.emplace_back( mp );
	}
}

void ReflectedMethodCommandParametersController::setMethodPath(const std::string& value)
{
	getParamObj()->setPath( value.c_str() );
}

const char* ReflectedMethodCommandParametersController::getMethodPath() const
{
	return getParamObj()->getPath();
}

void ReflectedMethodCommandParametersController::getObject( int * o_EnumValue ) const
{
	RefObjectId contextId = getParamObj()->getId();
	auto it = std::find_if( enumMap_.begin(), enumMap_.end(),
		[&]( const EnumMap::value_type& v ){ return v.first.find( contextId.toWString() ) != std::wstring::npos; } );

	if (it != enumMap_.end())
	{
		*o_EnumValue = (int)std::distance( enumMap_.begin(), it);
	}
}

void ReflectedMethodCommandParametersController::setObject( const int & o_EnumValue )
{
	auto it = enumMap_.begin();
	std::advance( it, o_EnumValue );
	getParamObj()->setId( it->second );
}

void ReflectedMethodCommandParametersController::generateObjList( std::map< int, std::wstring > * o_enumMap ) const
{
	enumMap_.clear();
	std::string path = getMethodPath();

	filterObjects( path, defMngr_, enumMap_ );

	o_enumMap->clear();
	for (auto it = enumMap_.begin(); it != enumMap_.end(); ++it)
	{
		o_enumMap->insert( std::pair<int, std::wstring>( static_cast< int >( std::distance( enumMap_.begin(), it ) ), it->first) );
	}
}

void MethodParam::setValue( const std::string& value )
{
	std::istringstream stream( value );
	stream >> *value_;
}

std::string MethodParam::getValue() const
{
	std::ostringstream stream;
	stream << *value_;
	std::string str = stream.str();
	return str;
}
} // end namespace wgt
