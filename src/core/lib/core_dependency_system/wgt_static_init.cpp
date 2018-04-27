#include "wgt_static_init.hpp"

namespace wgt
{
extern void registerStatic(
	const std::shared_ptr< IStaticInitalizer::ExecFunc > & execFunc,
	const std::shared_ptr< IStaticInitalizer::FiniFunc > & finiFunc
);

//==============================================================================
WGTStaticExecutor::WGTStaticExecutor(
	const IStaticInitalizer::ExecFunc& execFunc,
	const IStaticInitalizer::FiniFunc & finiFunc )
	: execFunc_( new IStaticInitalizer::ExecFunc( execFunc ) )
	, finiFunc_( new IStaticInitalizer::FiniFunc( finiFunc ) )
{
	registerStatic(execFunc_, finiFunc_);
}


//------------------------------------------------------------------------------
WGTStaticExecutor::WGTStaticExecutor()
{
}


//------------------------------------------------------------------------------
void WGTStaticExecutor::initFunc(
	const IStaticInitalizer::ExecFunc & execFunc,
	const IStaticInitalizer::FiniFunc & finiFunc)
{
	execFunc_.reset( new IStaticInitalizer::ExecFunc(execFunc) );
	finiFunc_.reset( new IStaticInitalizer::FiniFunc(finiFunc) );

	registerStatic( execFunc_, finiFunc_ );
}

} // end namespace wgt
