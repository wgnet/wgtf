#include "pch.hpp"

#include <iostream>
#include <fstream>

#include "cstdmf/guard.hpp"
#include "cstdmf/bw_util.hpp"
#include "cstdmf/debug.hpp"
#include "cstdmf/debug_filter.hpp"

#include "unit_test_lib/unit_test.hpp"

#include "reflection/class_definition.hpp"
#include "reflection/reflected_property.hpp"
#include "reflection/function_property.hpp"
#include "reflection/reflected_object.hpp"
#include "reflection/property_accessor.hpp"
#include "reflection/metadata/meta_types.hpp"
#include "reflection/utilities/reflection_function_utilities.hpp"
#include "serialization/serializer/xml_serializer.hpp"

#include "test_helpers.hpp"
#include "test_objects.hpp"

// -----------------------------------------------------------------------------
namespace wgt
{
TEST_F(TestDefinitionFixture, serialize_nothing)
{
	ENABLE_ASSERT_EXCEPTIONS;

	std::stringstream os;
	CHECK(os.good());
	{
		XMLSerializer xs(static_cast<std::ostream&>(os));
	}
	CHECK(!os.str().empty());

	std::stringstream is(os.str());
	CHECK(is.good());
	{
		XMLSerializer xs(static_cast<std::istream&>(is));
		BW::vector<ClassDefinition*> defs;
		CHECK(xs.readClassDefs(this->getDefinitionManager(), defs));
		CHECK(defs.empty());
		defs.clear();
		CHECK(xs.readClassDefs(this->getDefinitionManager(), defs));
		CHECK(defs.empty());

		BW::vector<ReflectedObjectPtr> objs;
		CHECK(xs.readObjects(this->getDefinitionManager(), objs, this->getObjectManager()));
		CHECK(objs.empty());
		objs.clear();
		CHECK(xs.readObjects(this->getDefinitionManager(), objs, this->getObjectManager()));
		CHECK(objs.empty());
	}
}

// -----------------------------------------------------------------------------
TEST_F(TestDefinitionFixture, serialize_simple_objects)
{
	ENABLE_ASSERT_EXCEPTIONS;

	ReflectedObjectPointer<TestDefinitionDerivedObject> pLastObj;
	BW::vector<ReflectedObjectPtr> objs;
	for (int i = 0; i < 100; ++i)
	{
		ReflectedObjectPointer<TestDefinitionDerivedObject> pObj =
		derived_klass_->createAndCastObject<TestDefinitionDerivedObject>();
		CHECK(pObj != NULL);

		pObj->initialise(i, pLastObj);

		if (pLastObj != NULL)
		{
			pLastObj->links_.push_back(pObj);
		}

		objs.push_back(pObj);

		pLastObj = pObj;
	}

	std::ofstream os("serialize_simple_objects.xml", std::ios::out | std::ios::trunc);
	CHECK(os.good());
	{
		XMLSerializer xs(static_cast<std::ostream&>(os));

		CHECK(xs.writeObjects(this->getDefinitionManager(), objs));
	}
	os.close();

	std::ifstream is("serialize_simple_objects.xml");
	CHECK(is.good());
	{
		XMLSerializer xs(is);

		BW::vector<ReflectedObjectPtr> objs_copy;
		CHECK(xs.readObjects(this->getDefinitionManager(), objs_copy, this->getObjectManager()));
		CHECK(objs.size() == objs_copy.size());

		for (int i = 0; i < objs_copy.size(); ++i)
		{
			const TestDefinitionDerivedObject* pObj1 = dynamic_cast<TestDefinitionDerivedObject*>(objs_copy[i].get());
			CHECK(pObj1);

			const TestDefinitionDerivedObject* pObj2 = dynamic_cast<TestDefinitionDerivedObject*>(objs[i].get());

			CHECK(*pObj1 == *pObj2);
		}
	}
	is.close();
}

// -----------------------------------------------------------------------------
TEST_F(TestDefinitionFixture, serialize_properties)
{
	ENABLE_ASSERT_EXCEPTIONS;

	ReflectedObjectPointer<TestDefinitionDerivedObject> pObj =
	derived_klass_->createAndCastObject<TestDefinitionDerivedObject>();
	CHECK(pObj != NULL);

	pObj->initialise(77, NULL);

	BW::vector<const PropertyAccessor> props;
	const PropertyAccessor& pa = derived_klass_->bindProperty("text", pObj.get());
	props.push_back(derived_klass_->bindProperty("text", pObj.get()));
	props.push_back(derived_klass_->bindProperty("counter", pObj.get()));

	std::ofstream os("serialize_properties.xml", std::ios::out | std::ios::trunc);
	CHECK(os.good());
	{
		XMLSerializer xs(static_cast<std::ostream&>(os));

		CHECK(xs.writeProperties(props));
	}
	os.close();

	std::ifstream is("serialize_properties.xml");
	CHECK(is.good());
	{
		XMLSerializer xs(is);

		BW::vector<PropertyAccessor> props_copy;
		xs.readProperties(props_copy, this->getObjectManager());

		CHECK_EQUAL(props.size(), props_copy.size());

		ReflectedObjectPointer<TestDefinitionObject> pObj_copy;
		if (!props_copy.empty())
		{
			pObj_copy = props_copy.front().getRootObject().get();
		}
		CHECK(pObj_copy != NULL);
		CHECK(pObj_copy.get() == pObj.get());
		CHECK_EQUAL(pObj->counter_, pObj_copy->counter_);
		CHECK_EQUAL(pObj->text_, pObj_copy->text_);
		CHECK(pObj->string_ == pObj_copy->string_);
	}
	is.close();
}
} // end namespace wgt
