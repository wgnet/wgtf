#ifndef PLUGIN1_OBJECTS_HPP
#define PLUGIN1_OBJECTS_HPP

#include <vector>
#include "core_reflection/reflected_object.hpp"

#include "core_generic_plugin_manager/unit_test/plugin2_test/plugin_objects.hpp"

namespace wgt
{
class TestPlugin1TestObject;
class TestPlugin1Interface;
typedef ObjectHandleT<TestPlugin1TestObject> TestPlugin1TestObjectPtr;
typedef ManagedObject<TestPlugin1TestObject> TestPlugin1TestObjectObj;
typedef ObjectHandleT<TestPlugin1Interface> TestPlugin1InterfacePtr;
typedef ManagedObject<TestPlugin1Interface> TestPlugin1InterfaceObj;

//------------------------------------------------------------------------------
class TestPlugin1TestObject
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

	bool operator==(const TestPlugin1TestObject& tdo) const
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

class ITestPlugin1
{
public:
	virtual ~ITestPlugin1()
	{
	}
	virtual TestPlugin1TestObjectPtr getObject() = 0;
	virtual TestPlugin2TestObjectPtr getObjectFromPlugin2() = 0;
	virtual void setObjectFromPlugin2(TestPlugin2TestObjectPtr obj) = 0;
};

//------------------------------------------------------------------------------
class TestPlugin1Interface : public Implements<ITestPlugin1>
{
	DECLARE_REFLECTED

public:
    TestPlugin1Interface() = default;
    virtual ~TestPlugin1Interface() = default;

	TestPlugin1TestObjectPtr getObject()
	{
		if (object_ == nullptr)
		{
			object_ = TestPlugin1TestObjectObj::make();
		}
        return object_.getHandleT();
	}

	TestPlugin2TestObjectPtr getObjectFromPlugin2()
	{
		return object2_;
	}

	void setObjectFromPlugin2(TestPlugin2TestObjectPtr obj)
	{
		object2_ = obj;
	}

private:
    TestPlugin1TestObjectObj object_;
	TestPlugin2TestObjectPtr object2_;
};

} // end namespace wgt
#endif // PLUGIN1_OBJECTS_HPP
