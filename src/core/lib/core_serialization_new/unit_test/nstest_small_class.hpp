#pragma once
#include <string>

namespace wgt
{
class NSTestSmallClass
{
public:
	NSTestSmallClass();
	~NSTestSmallClass();

	std::string& getFirstPref();
	std::string& getSecondPref();
	std::string& getThirdPref();

	void setFirstPref(const char* value);
	void setSecondPref(const char* value);
	void setThirdPref(const char* value);

private:
	std::string firstPref_;
	std::string secondPref_;
	std::string thirdPref_;
};

static bool operator==(NSTestSmallClass& lhs, NSTestSmallClass& rhs)
{
	if (lhs.getFirstPref().compare(rhs.getFirstPref()) != 0)
	{
		return false;
	}
	if (lhs.getSecondPref().compare(rhs.getSecondPref()) != 0)
	{
		return false;
	}
	if (lhs.getThirdPref().compare(rhs.getThirdPref()) != 0)
	{
		return false;
	}
	return true;
}

static bool operator!=(NSTestSmallClass& lhs, NSTestSmallClass& rhs)
{
	return !(lhs == rhs);
}
}