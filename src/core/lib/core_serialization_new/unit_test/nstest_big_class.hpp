#pragma once
#include <stdint.h>
#include <string>
#include "nstest_small_class.hpp"

namespace wgt
{
class NSTestBigClass
{
public:
	NSTestBigClass();
	~NSTestBigClass();

	const std::string& getName();
	const std::string& getString();
	intmax_t getCount();
	bool getCondition();
	double getPoint();
	NSTestSmallClass& getChild();

	void setName(const std::string& name);
	void setString(const std::string& string);
	void setCount(intmax_t count);
	void setCondition(bool condition);
	void setPoint(double point);
	void setChild(NSTestSmallClass child);

private:
	std::string name_;
	std::string string_;
	intmax_t count_;
	bool condition_;
	double point_;
	NSTestSmallClass child_;
};

static bool operator==(NSTestBigClass& lhs, NSTestBigClass& rhs)
{
	if (strcmp(lhs.getName().c_str(), rhs.getName().c_str()) != 0)
	{
		return false;
	}
	if (lhs.getString().compare(rhs.getString()) != 0)
	{
		return false;
	}
	if (lhs.getCount() != rhs.getCount())
	{
		return false;
	}
	if (lhs.getPoint() != rhs.getPoint())
	{
		return false;
	}
	if (lhs.getCondition() != rhs.getCondition())
	{
		return false;
	}
	if (lhs.getChild() != rhs.getChild())
	{
		return false;
	}
	return true;
}
}