#include "test_methods_object.hpp"


namespace wgt
{
void TestMethodsObject::testMethod1()
{
}


std::string TestMethodsObject::testMethod2()
{
	return "test2";
}


std::string TestMethodsObject::testMethod3( int parameter )
{
	return "test3";
}


std::string TestMethodsObject::testMethod4( const std::string& parameter1, int parameter2 )
{
	return "test4";
}


std::string TestMethodsObject::testMethod5( std::string& parameter )
{
	parameter = "test5";
	return "test5";
}


void TestMethodsObject::testMethod6( std::string* parameter )
{
	*parameter = "test6";
}


int TestMethodsObject::testMethod7( int parameter )
{
	return parameter;
}


double TestMethodsObject::testMethod8( double parameter )
{
	return parameter;
}

const std::string& TestMethodsObject::testMethod9(const std::string& parameter)
{
	testMethod9Str_ = parameter;
	return testMethod9Str_;
}

const std::string* TestMethodsObject::testMethod10(const std::string* parameter)
{
	testMethod10Str_ = *parameter;
	return &testMethod10Str_;
}

std::string& TestMethodsObject::testMethod11(std::string& parameter)
{
	testMethod11Str_ = parameter;
	return testMethod11Str_;
}

std::string* TestMethodsObject::testMethod12(std::string* parameter)
{
	testMethod12Str_ = *parameter;
	return &testMethod12Str_;
}
} // end namespace wgt
