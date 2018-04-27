#ifndef STRING_LIST_HPP
#define STRING_LIST_HPP

#include <stddef.h>
#include "interfaces/i_string_list.hpp"

namespace wgt
{

/**
*	Long test data.
*/
struct StringList
	: public IStringList
{
public:
	StringList();
	TestDataCoWString next() override;
private:
	size_t position_;
};

/**
*	Shorter test data than StringList.
*/
struct StringList2
	: public IStringList
{
public:
	StringList2();
	TestDataCoWString next() override;
private:
	size_t position_;
};

}

#endif //STRING_LIST_HPP