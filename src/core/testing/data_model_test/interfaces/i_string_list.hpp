#ifndef I_STRING_LIST_HPP
#define I_STRING_LIST_HPP

namespace wgt
{

class TestDataCoWString;

class IStringList
{
public:
	virtual ~IStringList() {}
	virtual TestDataCoWString next() = 0;
};

}//end namespace wgt

#endif //I_STRING_LIST_HPP