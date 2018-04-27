#include "pch.hpp"
#include "nstest_small_class.hpp"

namespace wgt
{
NSTestSmallClass::NSTestSmallClass()
    : firstPref_("MyCoolPref"), secondPref_("MyLessCoolPref"), thirdPref_("MyBoringPref")
{
}
NSTestSmallClass::~NSTestSmallClass()
{
}

std::string& NSTestSmallClass::getFirstPref()
{
	return firstPref_;
}
std::string& NSTestSmallClass::getSecondPref()
{
	return secondPref_;
}
std::string& NSTestSmallClass::getThirdPref()
{
	return thirdPref_;
}

void NSTestSmallClass::setFirstPref(const char* value)
{
	firstPref_ = value;
}
void NSTestSmallClass::setSecondPref(const char* value)
{
	secondPref_ = value;
}
void NSTestSmallClass::setThirdPref(const char* value)
{
	thirdPref_ = value;
}
}