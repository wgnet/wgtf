#ifndef TEST_ABSTRACT_ITEM_MODEL_HPP
#define TEST_ABSTRACT_ITEM_MODEL_HPP

#include "core_data_model/abstract_item_model.hpp"

namespace wgt
{
struct TestAbstractItemModelFixtureBase
{
	bool testInsertRows(TestResult& result_, const char*& m_name);
	bool testInsertColumns(TestResult& result_, const char*& m_name);
	bool testRemoveRows(TestResult& result_, const char*& m_name);
	bool testRemoveColumns(TestResult& result_, const char*& m_name);

	virtual AbstractItemModel* getModel() = 0;
};

template <typename T>
struct TestAbstractItemModelFixture : public TestAbstractItemModelFixtureBase
{
	AbstractItemModel* getModel() override
	{
		return &model_;
	}

	T model_;
};
}

#define ABSTRACT_ITEM_MODEL_TESTS \
	X(InsertRows)                 \
	X(InsertColumns)              \
	X(RemoveRows)                 \
	X(RemoveColumns)

#define ABSTRACT_ITEM_MODEL_TEST_F(FIXTURE, TEST)  \
	TEST_F(FIXTURE, TEST)                          \
	{                                              \
		bool result = test##TEST(result_, m_name); \
		CHECK(result);                             \
	}

/*
Use the following as an example of how to add unit tests for an abstract item model to your plugin

#define X( TEST ) ABSTRACT_ITEM_MODEL_TEST_F( TestAbstractItemModelFixture< MyItemModel >, TEST )
    ABSTRACT_ITEM_MODEL_TESTS
#undef X

If your model does not have a default constructor, inherit from TestAbstractItemModelFixture
to provide default arguments to your type's constructor
*/

#endif // TEST_ABSTRACT_ITEM_MODEL_HPP