#pragma once
#ifndef NSTEST_REFLECTED_CLASSES_HPP
#define NSTEST_REFLECTED_CLASSES_HPP
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/utilities/reflection_function_utilities.hpp"
#include <stdint.h>
#include <string>
#include <stack>

namespace wgt
{
class ReflectedTestMemberObject
{
	DECLARE_REFLECTED
public:
	ReflectedTestMemberObject() : name_("none"), enabled_(false), value_(0)
	{
	}

	ReflectedTestMemberObject(std::string name, bool enabled, int value) : name_(name), enabled_(enabled), value_(value)
	{
	}

	virtual ~ReflectedTestMemberObject()
	{
	}

	void setName(std::string newName)
	{
		name_ = newName;
	}

	void setEnabled(bool newEnabled)
	{
		enabled_ = newEnabled;
	}

	void setValue(int newValue)
	{
		value_ = newValue;
	}

	std::string getName()
	{
		return name_;
	}

	bool getEnabled()
	{
		return enabled_;
	}

	int getValue()
	{
		return value_;
	}

private:
	std::string name_;
	bool enabled_;
	int value_;
};

class ReflectedTestBaseObject
{
	DECLARE_REFLECTED
public:
	ReflectedTestBaseObject() : stringValue_("none"), intValue_(0), doubleValue_(0.0)
	{
	}
	virtual ~ReflectedTestBaseObject()
	{
	}

	const std::string& getStringValue()
	{
		return stringValue_;
	}

	intmax_t getIntValue()
	{
		return intValue_;
	}

	double getDoubleValue()
	{
		return doubleValue_;
	}

	uintmax_t getUintValue()
	{
		return uintValue_;
	}

	void setStringValue(const char* newString)
	{
		stringValue_ = newString;
	}
	void setIntValue(intmax_t newInt)
	{
		intValue_ = newInt;
		setDoubleValue(static_cast<double>(intValue_));
		setUintValue(static_cast<uintmax_t>(intValue_));
	}

	ReflectedTestMemberObject& getMemberObject()
	{
		return object_;
	}

private:
	void setDoubleValue(double newDouble)
	{
		doubleValue_ = newDouble;
	}

	void setUintValue(uintmax_t newUint)
	{
		uintValue_ = newUint;
	}

	std::string stringValue_;
	intmax_t intValue_;
	double doubleValue_;
	uintmax_t uintValue_;
	ReflectedTestMemberObject object_;
};

class ReflectedTestChildObject : public ReflectedTestBaseObject
{
	DECLARE_REFLECTED
public:
	ReflectedTestChildObject() : name_(L"nameless")
	{
	}
	ReflectedTestChildObject(const wchar_t* name) : ReflectedTestBaseObject(), name_(name)
	{
	}

	const std::wstring& getName()
	{
		return name_;
	}
	void setName(const wchar_t* newName)
	{
		name_ = newName;
	}

	void setChar(char newChar)
	{
		char_ = newChar;
	}

	char getChar()
	{
		return char_;
	}

	void setWChar(wchar_t newWChar)
	{
		wideChar_ = newWChar;
	}

	wchar_t getWChar()
	{
		return wideChar_;
	}
private:
	std::wstring name_;
	char char_;
	wchar_t wideChar_;
};

// Create class definitions.
BEGIN_EXPOSE(ReflectedTestBaseObject, MetaNone())
EXPOSE("StringValue", stringValue_, MetaNone())
EXPOSE("IntValue", intValue_, MetaNone())
EXPOSE("DoubleValue", doubleValue_, MetaNone())
EXPOSE("UintValue", uintValue_, MetaNone())
EXPOSE("MemberObject", object_, MetaNone())
EXPOSE_METHOD("SetDoubleValue", setDoubleValue)
END_EXPOSE()

BEGIN_EXPOSE(ReflectedTestChildObject, ReflectedTestBaseObject, MetaNone())
EXPOSE("Name", name_, MetaNone())
EXPOSE("Char", char_, MetaNone())
EXPOSE("WideChar", wideChar_, MetaNone())
END_EXPOSE()

BEGIN_EXPOSE(ReflectedTestMemberObject, MetaNone())
EXPOSE("Name", name_, MetaNone())
EXPOSE("Enabled", enabled_, MetaNone())
EXPOSE("Value", value_, MetaNone())
END_EXPOSE()
}

#endif // NSTEST_REFLECTED_CLASSES_HPP