#include "pch.hpp"

#include "test_abstract_item_model.hpp"
#include "core_data_model/collection_model.hpp"

namespace wgt
{
template< typename T >
struct TestCollectionModelFixture : public TestAbstractItemModelFixture< CollectionModel >
{
	TestCollectionModelFixture()
	{
		Collection collection( collection_ );
		model_.setSource( collection );
	}

	T collection_;
};

struct TestLinearCollectionModelFixture : public TestCollectionModelFixture< std::vector< int > > {};
struct TestMapCollectionModelFixture : public TestCollectionModelFixture< std::map< int, int > > {};

#define X( TEST ) ABSTRACT_ITEM_MODEL_TEST_F( TestLinearCollectionModelFixture, TEST )
	ABSTRACT_ITEM_MODEL_TESTS
#undef X

#define X( TEST ) ABSTRACT_ITEM_MODEL_TEST_F( TestMapCollectionModelFixture, TEST )
	ABSTRACT_ITEM_MODEL_TESTS
#undef X
}