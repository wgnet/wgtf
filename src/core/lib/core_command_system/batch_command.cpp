#include "batch_command.hpp"
#include "core_reflection/object_handle.hpp"
#include "command_manager.hpp"


namespace wgt
{
//--------------------------------------------------------------------------
BatchCommand::BatchCommand( CommandManager * pCommandManager )
    : pCommandManager_( pCommandManager )
{
}


//--------------------------------------------------------------------------
const char * BatchCommand::getId() const
{
    static const char * s_id = typeid( BatchCommand ).name();
    return s_id;
}


//--------------------------------------------------------------------------
ObjectHandle BatchCommand::execute( const ObjectHandle & arguments ) const
{
    assert( pCommandManager_ != nullptr );
	auto stage = arguments.getBase<BatchCommandStage>();
	assert( stage != nullptr );
	switch( *stage )
	{
	case BatchCommandStage::Begin:
		pCommandManager_->notifyBeginMultiCommand();
		break;
	case BatchCommandStage::End:
		break;
	case BatchCommandStage::Abort:
		return CommandErrorCode::ABORTED;
	default:
		assert( false );
		break;
	}
    return nullptr;
}


//--------------------------------------------------------------------------
CommandThreadAffinity BatchCommand::threadAffinity() const
{ 
	return CommandThreadAffinity::ANY_THREAD;
}
} // end namespace wgt
