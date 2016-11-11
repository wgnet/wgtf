#include "pch.hpp"
#include "multi_proc_test_case.hpp"

#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <memory>

namespace wgt
{
// -----------------------------------------------------------------------------
// Section: MultiProcTestCase Implementation
// -----------------------------------------------------------------------------

/**
 *	Creates the specified number of child processes for them to run child
 *	instances created from the child instance factory. This is intended for
 *	subclasses of MultiProcTestCase to use in their test cases for spawning
 *	multiple child processes.
 */
void MultiProcTestCase::runChildren(int num, MultiProcTestCase::ChildProcessFactory* pFactory)
{
	int numCreated = 0;
	while (numCreated < num)
	{
		++numCreated;

		// This call is responsible for the new child process object
		if (!this->runChild(pFactory->create()))
		{
			return; // RIP OUT FACTORY CODE
		}
	}
}

/**
 *	This method runs the input ChildProcess object in a forked process. This
 *	method is responsible for deleting this child process object.
 */
bool MultiProcTestCase::runChild(ChildProcess* pChildProcess)
{
	return false;
}

/**
 *  This method terminates all currently running children.
 */
void MultiProcTestCase::killChildren()
{
}

/**
 *	This method updates the child process map.
 *
 *	@param shouldBlock If set to true, this method will block on waitpid.
 */
bool MultiProcTestCase::updateChildren(bool shouldBlock)
{
	return false;
}

/**
 *	This method waits (blocks) for all pending child processes to exit. Call
 *	this after creating children via runChildren() or runChild().
 */
void MultiProcTestCase::waitForAll()
{
	// Let the main process run to completion.
	mainProcess_.run();

	// If the main process failed, terminate all children immediately.
	if (mainProcess_.hasFailed())
	{
		this->killChildren();
	}

	// Wait till all children complete.
	while (!pids_.empty())
	{
		this->updateChildren(/* shouldBlock: */ true);
	}
}

/**
 *	This method returns whether there are any running child processes.
 */
bool MultiProcTestCase::hasRunningChildren()
{
	return this->numRunningChildren() != 0;
}

/**
 *	This method returns the number of running child processes.
 */
int MultiProcTestCase::numRunningChildren()
{
	while (this->updateChildren(/* shouldBlock: */ false))
	{
		// pass
	}

	return static_cast<int>(pids_.size());
}

/**
 *	This method returns true if all children have passed. Call this only after
 *	calling runChildren() or runChild(). If some children do not pass, then it
 *	raises an CPPUnit assertion.
 */
bool MultiProcTestCase::checkAllChildrenPass()
{
	// Force failure on Win32 until this gets implemented.
	this->fail("Not implemented in Win32");
	return false;
}
} // end namespace wgt
