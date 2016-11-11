#include "pch.hpp"

#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/reflected_property.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/property_iterator.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/definition_manager.hpp"
#include "core_reflection/object_manager.hpp"
#include "core_reflection/reflected_method_parameters.hpp"
#include "core_reflection/unit_test/test_reflection_fixture.hpp"
#include "core/testing/reflection_objects_test/test_objects.hpp"
#include "core/testing/reflection_objects_test/test_methods_object.hpp"

namespace wgt
{
struct TestMethodsFixture : public TestReflectionFixture
{
	TestMethodsFixture()
	{
		IDefinitionManager& definitionManager = getDefinitionManager();
		klass_ = definitionManager.getDefinition<TestMethodsObject>();
	}

	IBasePropertyPtr findProperty(PropertyIterator& itr, const std::string& name)
	{
		IBasePropertyPtr property = *itr;
		std::string propertyName = property == nullptr ? "" : property->getName();

		while (propertyName != name && property != nullptr)
		{
			++itr;
			property = *itr;
			propertyName = property == nullptr ? "" : property->getName();
		}

		return propertyName == name ? property : nullptr;
	}

public:
	IClassDefinition* klass_;
};

TEST_F(TestMethodsFixture, methods)
{
	ObjectHandle object = klass_->createManagedObject();
	ReflectedMethodParameters parameters;

	auto pa = klass_->bindProperty("TestMethod1", object);
	CHECK(pa.isValid());
	Variant result = pa.invoke(parameters);

	pa = klass_->bindProperty("TestMethod2", object);
	CHECK(pa.isValid());
	result = pa.invoke(parameters);

	pa = klass_->bindProperty("TestMethod3", object);
	CHECK(pa.isValid());
	parameters.push_back(int(5));
	result = pa.invoke(parameters);
	std::string testResult;
	result.tryCast(testResult);
	CHECK(testResult == "test3");

	pa = klass_->bindProperty("TestMethod4", object);
	CHECK(pa.isValid());
	std::string parameterString = "test";
	ObjectHandle parameter1 = &parameterString;
	parameters = Variant(parameter1), Variant(5);
	result = pa.invoke(parameters);
	ObjectHandle checkHandle = parameters[0].cast<ObjectHandle>();
	testResult = *checkHandle.getBase<std::string>();
	CHECK(testResult == "test");
	CHECK(parameterString == "test");

	pa = klass_->bindProperty("TestMethod5", object);
	CHECK(pa.isValid());
	parameters = Variant(parameter1);
	result = pa.invoke(parameters);
	checkHandle = parameters[0].cast<ObjectHandle>();
	testResult = *checkHandle.getBase<std::string>();
	CHECK(testResult == "test5");
	CHECK(parameterString == "test5");

	pa = klass_->bindProperty("TestMethod6", object);
	CHECK(pa.isValid());
	parameters = Variant(&parameterString);
	pa.invoke(parameters);
	testResult = *parameters[0].cast<std::string*>();
	CHECK(testResult == "test6");
	CHECK(parameterString == "test6");

	parameters.clear();

	pa = klass_->bindProperty("TestMethod7", object);
	CHECK(pa.isValid());
	result = pa.invoke(Variant(double(4.4)));
	int testIntResult = result.value<int>();
	CHECK(testIntResult == 4);

	pa = klass_->bindProperty("TestMethod8", object);
	CHECK(pa.isValid());
	result = pa.invoke(Variant(int(5)));
	double testDoubleResult = result.value<double>();
	CHECK(testDoubleResult == 5.0);

	pa = klass_->bindProperty("TestMethod9", object);
	CHECK(pa.isValid());
	const std::string& constParameterString = parameterString;
	parameters = Variant(constParameterString);
	result = pa.invoke(parameters);
	const std::string& testConstStrResult = result.cast<const std::string&>();
	CHECK(testConstStrResult == parameterString);

	pa = klass_->bindProperty("TestMethod10", object);
	CHECK(pa.isValid());
	const std::string* constParameterPtrString = &parameterString;
	parameters = Variant(constParameterPtrString);
	result = pa.invoke(parameters);
	const std::string* testConstStrPtrResult = result.cast<const std::string*>();
	CHECK(*testConstStrPtrResult == parameterString);

	pa = klass_->bindProperty("TestMethod11", object);
	CHECK(pa.isValid());
	parameters = Variant(parameterString);
	result = pa.invoke(parameters);
	std::string& testStrResult = result.cast<std::string&>();
	CHECK(testStrResult == parameterString);

	pa = klass_->bindProperty("TestMethod12", object);
	CHECK(pa.isValid());
	parameters = Variant(&parameterString);
	result = pa.invoke(parameters);
	std::string* testStrPtrResult = result.cast<std::string*>();
	CHECK(*testStrPtrResult == parameterString);
}
} // end namespace wgt
