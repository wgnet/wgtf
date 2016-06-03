#include "test_command.hpp"
#include "core_logging/logging.hpp"


//TODO: Switch to multiplatform wait handles, possibly C++11
#include "core_common/ngt_windows.hpp"


namespace wgt
{
//==============================================================================
const char * TestCommand1::getId() const
{
	const char * s_Id = getClassIdentifier<TestCommand1>();
	return s_Id;
}

//==============================================================================
ObjectHandle TestCommand1::execute( const ObjectHandle & arguments ) const
{
	// This function will log the progress once a second for three seconds

	NGT_TRACE_MSG( "TestCommand1::execute - start \n" );

	uint8_t waitCounter = 0;
	volatile HANDLE commandHandle = CreateEvent( 0, true, false, NULL );
	// One second wait with WaitForSingleObject call
	while( ( processCounter_ > waitCounter ) &&
		   ::WaitForSingleObject( commandHandle, 1000 ) )
	{
		NGT_TRACE_MSG( "TestCommand1::execute - progress \n" );
		++waitCounter;
	}

	NGT_TRACE_MSG( "TestCommand1::execute - end \n" );

	return nullptr;
}

//==============================================================================
const char * TestCommand2::getId() const
{
	const char * s_Id = getClassIdentifier<TestCommand2>();
	return s_Id;
}

//==============================================================================
ObjectHandle TestCommand2::execute( const ObjectHandle & arguments ) const
{
	// This function will log the progress once a second for five seconds

	NGT_TRACE_MSG( "TestCommand2::execute - start \n" );

	uint8_t waitCounter = 0;
	volatile HANDLE commandHandle = CreateEvent( 0, true, false, NULL );
	// One second wait with WaitForSingleObject call
	while( ( processCounter_ > waitCounter ) &&
		   ::WaitForSingleObject( commandHandle, 1000 ) )
	{
		NGT_TRACE_MSG( "TestCommand2::execute - progress \n" );
		++waitCounter;
	}

	NGT_TRACE_MSG( "TestCommand2::execute - end \n" );
	return nullptr;
}
} // end namespace wgt
