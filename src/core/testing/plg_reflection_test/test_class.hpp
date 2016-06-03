#ifndef TEST_CLASS_HPP
#define TEST_CLASS_HPP

#include "core_reflection/reflected_object.hpp"
#include <string>

namespace wgt
{
class TestBase
{
	DECLARE_REFLECTED
public:
	TestBase()
		: baseName_( "TestBaseName" )
	{}
private:
	const char* baseName_;
};

class TestClass : public TestBase
{
	DECLARE_REFLECTED
public:
	TestClass()
		: TestBase()
		, name_( "TestClassName" )
		, string_( "TestClassString" )
		, innerClass_( "TestClassInnerName" )
	{}
 
	const std::string& getString() const
		{ return string_; }
	void setString( const std::string& value )
		{ string_ = value; }

	/**
	 *	Inner classes cannot inherit from ReflectedObject.
	 */
	class InnerClass
	{
		DECLARE_REFLECTED
	public:
		InnerClass()
			: innerName_( "DefaultInnerName" )
		{}
		InnerClass( const char* name )
			: innerName_( name )
		{}
	private:
		const char* innerName_;
	};
 
private:
	const char* name_;
	std::string string_;
	InnerClass innerClass_;
};
} // end namespace wgt
#endif // TEST_CLASS_HPP
