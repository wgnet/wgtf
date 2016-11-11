#include "qt_ui_worker.hpp"

//------------------------------------------------------------------------------
void QtUIWorker::doJob(QtUIFunctionWrapper* wrappedFunc)
{
	wrappedFunc->func_();
	delete wrappedFunc;
}