#include "set_item_data_command.hpp"
#include "set_item_data_command_arg.hpp"

#include "core_command_system/i_command_manager.hpp"
#include "core_data_model/abstract_item_model.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_variant/variant.hpp"

namespace wgt
{
namespace SetItemDataCommand_Detail
{
bool isValid( const SetItemDataCommandArgument * pCommandArgs )
{
	if (pCommandArgs == nullptr) 
	{
		return false;
	}

	if (pCommandArgs->pModel_ == nullptr)
	{
		return false;
	}

	if (!pCommandArgs->index_.isValid())
	{
		return false;
	}

	return true;
}
} // end namespace SetItemDataCommand_Detail

SetItemDataCommand::SetItemDataCommand( IComponentContext & context )
	: definitionManager_( context )
{
}


bool SetItemDataCommand::customUndo() const /* override */
{
	return true;
}


bool SetItemDataCommand::canUndo( const ObjectHandle & arguments ) const /* override */
{
	return this->validateArguments( arguments );
}


bool SetItemDataCommand::undo( const ObjectHandle & arguments ) const /* override */
{
	if (!arguments.isValid()) 
	{
		return false;
	}

	auto pCommandArgs = arguments.getBase< SetItemDataCommandArgument >();
	if (!SetItemDataCommand_Detail::isValid(pCommandArgs))
	{
		return false;
	}

	auto & model = (*pCommandArgs->pModel_);
	const auto & index = (pCommandArgs->index_);
	const auto & roleId = (pCommandArgs->roleId_);
	const auto & oldValue = (pCommandArgs->oldValue_);

	auto pItem = model.item( index );
	assert( pItem != nullptr );
	return pItem->setData( index.row_, index.column_, roleId, oldValue );
}


bool SetItemDataCommand::redo( const ObjectHandle & arguments ) const /* override */
{
	if (!arguments.isValid()) 
	{
		return false;
	}

	auto pCommandArgs = arguments.getBase< SetItemDataCommandArgument >();
	if (!SetItemDataCommand_Detail::isValid(pCommandArgs))
	{
		return false;
	}

	auto & model = (*pCommandArgs->pModel_);
	const auto & index = (pCommandArgs->index_);
	const auto & roleId = (pCommandArgs->roleId_);
	const auto & newValue = (pCommandArgs->newValue_);

	auto pItem = model.item( index );
	assert( pItem != nullptr );
	return pItem->setData( index.row_, index.column_, roleId, newValue );
}


ObjectHandle SetItemDataCommand::getCommandDescription(
	const ObjectHandle & arguments ) const /* override */
{
	auto handle = GenericObject::create( *definitionManager_ );
	assert( handle.get() != nullptr );
	auto& genericObject = (*handle);

	if (!arguments.isValid()) 
	{
		genericObject.set( "Name", "Invalid" );
		genericObject.set( "Type", "Data" );
		return ObjectHandle( std::move( handle ) );
	}

	auto pCommandArgs = arguments.getBase< SetItemDataCommandArgument >();
	if (!SetItemDataCommand_Detail::isValid(pCommandArgs))
	{
		genericObject.set( "Name", "Invalid" );
		genericObject.set( "Type", "Data" );
		return ObjectHandle( std::move( handle ) );
	}

	genericObject.set( "Name", "Data" );
	genericObject.set( "Type", "Data" );

	const auto & oldValue = (pCommandArgs->oldValue_);
	const auto & newValue = (pCommandArgs->newValue_);
	genericObject.set( "PreValue", oldValue );
	genericObject.set( "PostValue", newValue );

	return ObjectHandle( std::move( handle ) );
}


const char * SetItemDataCommand::getId() const /* override */
{
	static const char * s_Id = wgt::getClassIdentifier< SetItemDataCommand >();
	return s_Id;
}


bool SetItemDataCommand::validateArguments(
	const ObjectHandle & arguments ) const /* override */
{
	const auto pCommandArgs = arguments.getBase< SetItemDataCommandArgument >();
	return SetItemDataCommand_Detail::isValid(pCommandArgs);
}


ObjectHandle SetItemDataCommand::execute(
	const ObjectHandle & arguments ) const /* override */
{
	auto pCommandArgs = arguments.getBase< SetItemDataCommandArgument >();
	if (!SetItemDataCommand_Detail::isValid(pCommandArgs))
	{
		return CommandErrorCode::INVALID_ARGUMENTS;
	}

	auto & model = (*pCommandArgs->pModel_);
	const auto & index = (pCommandArgs->index_);
	const auto & roleId = (pCommandArgs->roleId_);
	const auto & newValue = (pCommandArgs->newValue_);

	auto pItem = model.item( index );
	assert( pItem != nullptr );
	const auto result = pItem->setData( index.row_, index.column_, roleId, newValue );
	const auto errorCode = result ?
		CommandErrorCode::COMMAND_NO_ERROR :
		CommandErrorCode::FAILED;

	return errorCode;
}


CommandThreadAffinity SetItemDataCommand::threadAffinity() const /* override */
{
	return CommandThreadAffinity::UI_THREAD;
}


} // end namespace wgt
