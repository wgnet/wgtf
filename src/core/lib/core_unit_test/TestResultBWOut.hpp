#ifndef TESTRESULTBWOUT_H
#define TESTRESULTBWOUT_H

#include "CppUnitLite2/src/TestResult.h"
#include <string>


namespace wgt
{
class TestResultBWOut : public TestResult
{
public:
	TestResultBWOut( const std::string & name, bool useXML = false );

	virtual void StartTests ();
    virtual void AddFailure (const Failure & failure);
    virtual void EndTests ();

	void StartTestsXML();
	void AddFailureXML( const Failure & failure );
	void EndTestsXML();

	void StartTestsTXT();
	void AddFailureTXT( const Failure & failure );
	void EndTestsTXT();

private:
	std::string name_;
	bool usingXML_;
};
} // end namespace wgt
#endif // TESTRESULTBWOUT_H
