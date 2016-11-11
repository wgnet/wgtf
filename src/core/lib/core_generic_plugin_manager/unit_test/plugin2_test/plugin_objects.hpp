#ifndef PLUGIN2_OBJECTS_HPP
#define PLUGIN2_OBJECTS_HPP

#include <vector>
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_dependency_system/i_interface.hpp"

namespace wgt
{
//------------------------------------------------------------------------------
class TestPlugin2TestObject
{
	DECLARE_REFLECTED

public:
	int counter_;
	std::string text_;
	std::vector<float> values_;

	void setCounter(const int& value)
	{
		counter_ = value;
	}
	void getCounter(int* value) const
	{
		*value = counter_;
	}

	void setText(const std::string& value)
	{
		text_ = value;
	}
	const std::string& getText() const
	{
		return text_;
	}

	bool operator==(const TestPlugin2TestObject& tdo) const
	{
		if (counter_ != tdo.counter_ || text_ != tdo.text_ || values_.size() != tdo.values_.size())
		{
			return false;
		}

		size_t i = 0;
		for (; i < values_.size() && values_[i] == tdo.values_[i]; ++i)
			;
		return i == values_.size();
	}
};

typedef ObjectHandleT<TestPlugin2TestObject> TestPlugin2TestObjectPtr;

class ITestPlugin2
{
public:
	virtual ~ITestPlugin2()
	{
	}
	virtual TestPlugin2TestObjectPtr getObject(IDefinitionManager& defManager) = 0;
};

//------------------------------------------------------------------------------
class TestPlugin2Interface : public Implements<ITestPlugin2>
{
	DECLARE_REFLECTED

public:
	TestPlugin2Interface() : object_(NULL)
	{
	}

	TestPlugin2TestObjectPtr getObject(IDefinitionManager& defManager)
	{
		if (object_ == NULL)
		{
			object_ = defManager.create<TestPlugin2TestObject>();
		}
		return object_;
	}

private:
	TestPlugin2TestObjectPtr object_;
};

typedef ObjectHandleT<TestPlugin2Interface> TestPlugin2InterfacePtr;
} // end namespace wgt
#endif // PLUGIN2_OBJECTS_HPP
