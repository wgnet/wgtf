#include "pch.hpp"
#include "nstest_big_class.hpp"
#include "nstest_small_class.hpp"

namespace wgt
{
NSTestBigClass::NSTestBigClass()
    : name_("none"), string_("nothing"), count_(0), condition_(false), point_(0.), child_(NSTestSmallClass())
{
}

NSTestBigClass::~NSTestBigClass()
{
}

const std::string& NSTestBigClass::getName()
{
	return name_;
}

const std::string& NSTestBigClass::getString()
{
	return string_;
}

intmax_t NSTestBigClass::getCount()
{
	return count_;
}

bool NSTestBigClass::getCondition()
{
	return condition_;
}

double NSTestBigClass::getPoint()
{
	return point_;
}

NSTestSmallClass& NSTestBigClass::getChild()
{
	return child_;
}

void NSTestBigClass::setName(const std::string& name)
{
	name_ = name;
}

void NSTestBigClass::setString(const std::string& string)
{
	string_ = string;
}

void NSTestBigClass::setCount(intmax_t count)
{
	count_ = count;
}

void NSTestBigClass::setCondition(bool condition)
{
	condition_ = condition;
}

void NSTestBigClass::setPoint(double point)
{
	point_ = point;
}

void NSTestBigClass::setChild(NSTestSmallClass child)
{
	child_.setFirstPref(child.getFirstPref().c_str());
	child_.setSecondPref(child.getSecondPref().c_str());
	child_.setThirdPref(child.getThirdPref().c_str());
}

}