#ifndef TEST_DATA_COW_STRING_HPP
#define TEST_DATA_COW_STRING_HPP

#include <string>

namespace wgt
{

/***
 * This is just used to optimization of memory usage for test data.
 * Still needs proper copy semantics.
 */
class TestDataCoWString
{
public:
	TestDataCoWString(const char * str)
		: value_( const_cast< char * >( str ) )
		, owns_(false)
	{
	}


	~TestDataCoWString()
	{
		if (owns_)
		{
			delete[] value_;
		}
	}

	bool empty() const
	{
		return value_ == nullptr;
	}


	TestDataCoWString & operator=(const std::string & str)
	{
		if (owns_)
		{
			delete value_;
		}
		value_ = new char[str.size() + 1];
		strcpy( value_, str.c_str());
		owns_ = false;
		return *this;
	}

	const char * c_str() const
	{
		return value_;
	}

private:
	bool owns_;
	char * value_;
};

}//end namespace wgt

#endif //TEST_DATA_COW_STRING_HPP