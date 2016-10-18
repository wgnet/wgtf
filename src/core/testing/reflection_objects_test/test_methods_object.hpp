#ifndef TEST_METHODS_OBJECT_HPP
#define TEST_METHODS_OBJECT_HPP

#include "core_reflection/reflected_object.hpp"
#include "core_reflection/i_definition_manager.hpp"

namespace wgt
{
class TestMethodsObject
{
	DECLARE_REFLECTED

public:
	void testMethod1();
	std::string testMethod2();
	std::string testMethod3( int parameter );
	std::string testMethod4( const std::string& parameter1, int parameter2 );
	std::string testMethod5( std::string& parameter );
	void testMethod6( std::string* parameter );
	int testMethod7( int parameter );
	double testMethod8( double parameter );
	const std::string& testMethod9(const std::string& parameter);
	const std::string* testMethod10(const std::string* parameter);
	std::string& testMethod11(std::string& parameter);
	std::string* testMethod12(std::string* parameter);

private:
	std::string testMethod9Str_;
	std::string testMethod10Str_;
	std::string testMethod11Str_;
	std::string testMethod12Str_;
};
} // end namespace wgt
#endif //TEST_METHODS_OBJECT
