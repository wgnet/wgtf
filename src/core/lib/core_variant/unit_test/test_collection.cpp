#include "pch.hpp"

#include "core_variant/collection.hpp"
#include <map>
#include <vector>

#define EXTRA_ARGS_DECLARE TestResult &result_, const char *m_name
#define EXTRA_ARGS result_, m_name

namespace wgt
{
namespace
{
void iteratorSanityCheck(EXTRA_ARGS_DECLARE, const Collection& collection)
{
	size_t size = 0;
	auto itCheck = collection.begin();
	for (auto it = collection.begin(); it != collection.end(); ++it, itCheck++, ++size)
	{
		CHECK(it == itCheck);
		CHECK(itCheck == it);

		CHECK(it != collection.end());
		CHECK(collection.end() != it);

		CHECK(itCheck != collection.end());
		CHECK(collection.end() != itCheck);

		CHECK(it.value() == *it);
		CHECK(itCheck.value() == *itCheck);
		CHECK(*it == *itCheck);
		CHECK(it.value() == itCheck.value());
	}

	CHECK_EQUAL(size, collection.size());
}

template <typename Key, typename Value>
struct CheckValue
{
	Key key;
	Value value;
};

template <typename Container>
typename Container::mapped_type extractValue(const Container& container, const typename Container::key_type& key)
{
	auto it = container.find(key);
	if (it != container.end())
	{
		return it->second;
	}
	else
	{
		return typename Container::mapped_type();
	}
}

template <typename T, typename Allocator>
T extractValue(const std::vector<T, Allocator>& container, size_t key)
{
	return container[key];
}

template <typename T, size_t N>
T extractValue(const std::array<T, N>& container, size_t key)
{
	return container[key];
}

template <typename Container, typename Key, typename Value, size_t N>
void containerCheck(EXTRA_ARGS_DECLARE, const Container& container, const CheckValue<Key, Value> (&check)[N])
{
	CHECK(container.size() == N);

	for (size_t i = 0; i != N; ++i)
	{
		CHECK_EQUAL(check[i].value, extractValue(container, check[i].key));
	}

	CHECK(container.size() == N);
}

template <typename Key, typename Value, size_t N>
void collectionCheck(EXTRA_ARGS_DECLARE, const Collection& collection, const CheckValue<Key, Value> (&check)[N])
{
	CHECK(collection.size() == N);

	for (size_t i = 0; i != N; ++i)
	{
		auto it = collection.find(check[i].key);
		CHECK(it != collection.end());
		CHECK(*it == check[i].value);
		CHECK(it.value() == check[i].value);
		CHECK(it.key() == check[i].key);

		CHECK(collection[check[i].key] == check[i].value);
	}

	CHECK(collection.size() == N);
}

template <typename Key, typename Value, size_t N>
void collectionCheckOrdered(EXTRA_ARGS_DECLARE, const Collection& collection, const CheckValue<Key, Value> (&check)[N])
{
	CHECK(collection.size() == N);

	size_t i = 0;
	for (auto it = collection.begin(); it != collection.end(); ++it, ++i)
	{
		RETURN_ON_FAIL_CHECK(i < N);
		CHECK(it.key() == check[i].key);
		CHECK(it.value() == check[i].value);
		CHECK(*it == check[i].value);
	}

	CHECK(collection.size() == N);
}

template <typename Container>
struct FixtureBase
{
	FixtureBase() : container(), collection(container), constCollection(const_cast<const Container&>(container))
	{
	}

	template <typename Key, typename Value, size_t N>
	void checkContents(EXTRA_ARGS_DECLARE, const CheckValue<Key, Value> (&check)[N]) const
	{
		containerCheck(EXTRA_ARGS, container, check);
		collectionCheck(EXTRA_ARGS, collection, check);
		collectionCheck(EXTRA_ARGS, constCollection, check);
	}

	Container container;
	Collection collection;
	Collection constCollection;
};

struct VectorFixture : public FixtureBase<std::vector<int>>
{
	VectorFixture()
	{
		container.push_back(0);
		container.push_back(1);
		container.push_back(2);
	}
};

struct ArrayFixture : public FixtureBase<std::array<int, 3>>
{
	ArrayFixture() : FixtureBase<std::array<int, 3>>()
	{
		container[0] = 0;
		container[1] = 1;
		container[2] = 2;
	}
};

struct MapFixture : public FixtureBase<std::map<std::string, int>>
{
	MapFixture()
	{
		container["zero"] = 0;
		container["one"] = 1;
		container["two"] = 2;
	}
};
}

TEST_F(VectorFixture, Collection_vector)
{
	iteratorSanityCheck(EXTRA_ARGS, collection);
	iteratorSanityCheck(EXTRA_ARGS, constCollection);

	const CheckValue<size_t, int> check[] = { { 0, 0 }, { 1, 1 }, { 2, 2 } };

	checkContents(EXTRA_ARGS, check);
}

TEST_F(VectorFixture, Collection_vector_write_existing)
{
	collection[1] = 7;

	const CheckValue<size_t, int> check[] = { { 0, 0 }, { 1, 7 }, { 2, 2 } };

	checkContents(EXTRA_ARGS, check);
}

TEST_F(VectorFixture, Collection_vector_write_existing_iterator)
{
	auto it = collection.find(1);
	CHECK(it != collection.end());
	*it = 7;

	CheckValue<size_t, int> check[] = { { 0, 0 }, { 1, 7 }, { 2, 2 } };

	checkContents(EXTRA_ARGS, check);

	it.setValue(42);
	check[1].value = 42;

	checkContents(EXTRA_ARGS, check);
}

TEST_F(VectorFixture, Collection_vector_write_new)
{
	collection[3] = 7;

	const CheckValue<size_t, int> check[] = { { 0, 0 }, { 1, 1 }, { 2, 2 }, { 3, 7 } };

	checkContents(EXTRA_ARGS, check);
}

TEST_F(VectorFixture, Collection_vector_write_new_gapped)
{
	collection[4] = 7;

	const CheckValue<size_t, int> check[] = { { 0, 0 }, { 1, 1 }, { 2, 2 }, { 3, 0 }, { 4, 7 } };

	checkContents(EXTRA_ARGS, check);
}

TEST_F(VectorFixture, Collection_vector_insert_mid)
{
	auto it = collection.insert(1);
	CHECK(it != collection.end());
	*it = -1;

	const CheckValue<size_t, int> check[] = { { 0, 0 }, { 1, -1 }, { 2, 1 }, { 3, 2 } };

	checkContents(EXTRA_ARGS, check);
}

TEST_F(VectorFixture, Collection_vector_insert_gap)
{
	auto it = collection.insert(8);
	CHECK(it != collection.end());
	*it = 13;

	const CheckValue<size_t, int> check[] = { { 0, 0 }, { 1, 1 }, { 2, 2 }, { 3, 0 }, { 4, 0 },
		                                      { 5, 0 }, { 6, 0 }, { 7, 0 }, { 8, 13 } };

	checkContents(EXTRA_ARGS, check);
}

TEST_F(VectorFixture, Collection_vector_erase_iterator)
{
	auto it = collection.find(1);
	CHECK(it != collection.end());
	it = collection.erase(it);
	CHECK(it != collection.end());
	CHECK(*it == 2);

	const CheckValue<size_t, int> check[] = { { 0, 0 }, { 1, 2 } };

	checkContents(EXTRA_ARGS, check);
}

TEST_F(VectorFixture, Collection_vector_erase_iterator_range)
{
	auto rangeEnd = collection.find(2);
	CHECK(rangeEnd != collection.end());
	auto it = collection.erase(collection.begin(), rangeEnd);
	CHECK(it != collection.end());
	CHECK(*it == 2);

	const CheckValue<size_t, int> check[] = { { 0, 2 } };

	checkContents(EXTRA_ARGS, check);
}

TEST_F(VectorFixture, Collection_vector_erase_iterator_range_end)
{
	auto rangeBegin = collection.find(1);
	CHECK(rangeBegin != collection.end());
	auto it = collection.erase(rangeBegin, collection.end());
	CHECK(it == collection.end());

	const CheckValue<size_t, int> check[] = { { 0, 0 } };

	checkContents(EXTRA_ARGS, check);
}

TEST_F(VectorFixture, Collection_vector_erase_iterator_empty_range)
{
	auto it = collection.erase(collection.begin(), collection.begin());
	CHECK(it != collection.end());
	CHECK(*it == 0);

	const CheckValue<size_t, int> check[] = { { 0, 0 }, { 1, 1 }, { 2, 2 } };

	checkContents(EXTRA_ARGS, check);
}

TEST_F(VectorFixture, Collection_vector_erase_key)
{
	CHECK(collection.eraseKey(1) == 1);

	const CheckValue<size_t, int> check[] = { { 0, 0 }, { 1, 2 } };

	checkContents(EXTRA_ARGS, check);
}

TEST_F(VectorFixture, Collection_vector_const)
{
	const CheckValue<size_t, int> check[] = { { 0, 0 }, { 1, 1 }, { 2, 2 } };

	checkContents(EXTRA_ARGS, check);

	constCollection[1] = 7;
	checkContents(EXTRA_ARGS, check);

	auto it = constCollection.find(1);
	CHECK(it != constCollection.end());
	*it = 7;
	checkContents(EXTRA_ARGS, check);

	it.setValue(7);
	checkContents(EXTRA_ARGS, check);

	constCollection[3] = 7;
	checkContents(EXTRA_ARGS, check);

	constCollection[4] = 7;
	checkContents(EXTRA_ARGS, check);

	CHECK(constCollection.insert(1) == constCollection.end());
	checkContents(EXTRA_ARGS, check);

	CHECK(constCollection.insert(8) == constCollection.end());
	checkContents(EXTRA_ARGS, check);

	CHECK(constCollection.erase(it) == constCollection.end());
	checkContents(EXTRA_ARGS, check);

	CHECK(constCollection.eraseKey(1) == 0);
	checkContents(EXTRA_ARGS, check);
}

TEST_F(VectorFixture, Collection_vector_distance)
{
	{
		const auto it = std::find(constCollection.cbegin(), constCollection.cend(), 0);
		CHECK(it != constCollection.cend());
		const auto beginDiff = std::distance(constCollection.cbegin(), it);
		CHECK(beginDiff == 0);
		const auto endDiff = std::distance(it, constCollection.cend());
		CHECK(endDiff == 3);
	}

	{
		const auto it = std::find(constCollection.cbegin(), constCollection.cend(), 1);
		CHECK(it != constCollection.cend());
		const auto beginDiff = std::distance(constCollection.cbegin(), it);
		CHECK(beginDiff == 1);
		const auto endDiff = std::distance(it, constCollection.cend());
		CHECK(endDiff == 2);
	}

	{
		const auto it = std::find(constCollection.cbegin(), constCollection.cend(), 2);
		CHECK(it != constCollection.cend());
		const auto beginDiff = std::distance(constCollection.cbegin(), it);
		CHECK(beginDiff == 2);
		const auto endDiff = std::distance(it, constCollection.cend());
		CHECK(endDiff == 1);
	}

	{
		const auto it = std::find(constCollection.cbegin(), constCollection.cend(), 3);
		CHECK(it == constCollection.cend());
		const auto beginDiff = std::distance(constCollection.cbegin(), it);
		CHECK(beginDiff == 3);
		const auto endDiff = std::distance(it, constCollection.cend());
		CHECK(endDiff == 0);
	}
}

TEST_F(VectorFixture, Collection_vector_iterator)
{
	{
		auto it = constCollection.cbegin();
		CHECK(*it == 0);
		CHECK(it == constCollection.cbegin());
		CHECK(it != constCollection.cend());

		++it;
		CHECK(*it == 1);
		CHECK(it != constCollection.cbegin());
		CHECK(it != constCollection.cend());

		++it;
		CHECK(*it == 2);
		CHECK(it != constCollection.cbegin());
		CHECK(it != constCollection.cend());

		++it;
		CHECK(it != constCollection.cbegin());
		CHECK(it == constCollection.cend());
	}
}

////////////////////////////////////////////////////////////////////////////////

TEST_F(ArrayFixture, Collection_array)
{
	iteratorSanityCheck(EXTRA_ARGS, collection);
	iteratorSanityCheck(EXTRA_ARGS, constCollection);

	const CheckValue<size_t, int> check[] = { { 0, 0 }, { 1, 1 }, { 2, 2 } };

	checkContents(EXTRA_ARGS, check);
}

TEST_F(ArrayFixture, Collection_array_write_existing)
{
	collection[1] = 7;

	const CheckValue<size_t, int> check[] = { { 0, 0 }, { 1, 7 }, { 2, 2 } };

	checkContents(EXTRA_ARGS, check);
}

TEST_F(ArrayFixture, Collection_array_write_existing_iterator)
{
	auto it = collection.find(1);
	CHECK(it != collection.end());
	*it = 7;

	CheckValue<size_t, int> check[] = { { 0, 0 }, { 1, 7 }, { 2, 2 } };

	checkContents(EXTRA_ARGS, check);

	it.setValue(42);
	check[1].value = 42;

	checkContents(EXTRA_ARGS, check);
}

TEST_F(ArrayFixture, Collection_array_resize)
{
	const CheckValue<size_t, int> check[] = { { 0, 0 }, { 1, 1 }, { 2, 2 } };

	checkContents(EXTRA_ARGS, check);

	collection[3] = 7;
	checkContents(EXTRA_ARGS, check);

	collection[4] = 7;
	checkContents(EXTRA_ARGS, check);

	collection[8] = 7;
	checkContents(EXTRA_ARGS, check);

	CHECK(collection.insert(1) == collection.end());
	checkContents(EXTRA_ARGS, check);

	CHECK(collection.insert(8) == collection.end());
	checkContents(EXTRA_ARGS, check);

	auto it = collection.find(1);
	CHECK(it != collection.end());
	CHECK(collection.erase(it) == collection.end());
	checkContents(EXTRA_ARGS, check);

	CHECK(collection.eraseKey(1) == 0);
	checkContents(EXTRA_ARGS, check);
}

TEST_F(ArrayFixture, Collection_array_const)
{
	const CheckValue<size_t, int> check[] = { { 0, 0 }, { 1, 1 }, { 2, 2 } };

	checkContents(EXTRA_ARGS, check);

	constCollection[1] = 7;
	checkContents(EXTRA_ARGS, check);

	auto it = constCollection.find(1);
	CHECK(it != constCollection.end());
	*it = 7;
	checkContents(EXTRA_ARGS, check);

	it.setValue(7);
	checkContents(EXTRA_ARGS, check);

	constCollection[3] = 7;
	checkContents(EXTRA_ARGS, check);

	constCollection[4] = 7;
	checkContents(EXTRA_ARGS, check);

	CHECK(constCollection.insert(1) == constCollection.end());
	checkContents(EXTRA_ARGS, check);

	CHECK(constCollection.insert(8) == constCollection.end());
	checkContents(EXTRA_ARGS, check);

	CHECK(constCollection.erase(it) == constCollection.end());
	checkContents(EXTRA_ARGS, check);

	CHECK(constCollection.eraseKey(1) == 0);
	checkContents(EXTRA_ARGS, check);
}

TEST_F(ArrayFixture, Collection_array_distance)
{
	const CheckValue<size_t, int> check[] = { { 0, 0 }, { 1, 1 }, { 2, 2 } };

	checkContents(EXTRA_ARGS, check);

	{
		const auto it = std::find(constCollection.cbegin(), constCollection.cend(), 0);
		CHECK(it != constCollection.cend());
		const auto beginDiff = std::distance(constCollection.cbegin(), it);
		CHECK(beginDiff == 0);
		const auto endDiff = std::distance(it, constCollection.cend());
		CHECK(endDiff == 3);
		checkContents(EXTRA_ARGS, check);
	}

	{
		const auto it = std::find(constCollection.cbegin(), constCollection.cend(), 1);
		CHECK(it != constCollection.cend());
		const auto beginDiff = std::distance(constCollection.cbegin(), it);
		CHECK(beginDiff == 1);
		const auto endDiff = std::distance(it, constCollection.cend());
		CHECK(endDiff == 2);
		checkContents(EXTRA_ARGS, check);
	}

	{
		const auto it = std::find(constCollection.cbegin(), constCollection.cend(), 2);
		CHECK(it != constCollection.cend());
		const auto beginDiff = std::distance(constCollection.cbegin(), it);
		CHECK(beginDiff == 2);
		const auto endDiff = std::distance(it, constCollection.cend());
		CHECK(endDiff == 1);
		checkContents(EXTRA_ARGS, check);
	}

	{
		const auto it = std::find(constCollection.cbegin(), constCollection.cend(), 3);
		CHECK(it == constCollection.cend());
		const auto beginDiff = std::distance(constCollection.cbegin(), it);
		CHECK(beginDiff == 3);
		const auto endDiff = std::distance(it, constCollection.cend());
		CHECK(endDiff == 0);
		checkContents(EXTRA_ARGS, check);
	}
}

TEST_F(ArrayFixture, Collection_array_iterator)
{
	{
		auto it = constCollection.cbegin();
		CHECK(*it == 0);
		CHECK(it == constCollection.cbegin());
		CHECK(it != constCollection.cend());

		++it;
		CHECK(*it == 1);
		CHECK(it != constCollection.cbegin());
		CHECK(it != constCollection.cend());

		++it;
		CHECK(*it == 2);
		CHECK(it != constCollection.cbegin());
		CHECK(it != constCollection.cend());

		++it;
		CHECK(it != constCollection.cbegin());
		CHECK(it == constCollection.cend());
	}
}

////////////////////////////////////////////////////////////////////////////////

TEST_F(MapFixture, Collection_map)
{
	iteratorSanityCheck(EXTRA_ARGS, collection);
	iteratorSanityCheck(EXTRA_ARGS, constCollection);

	const CheckValue<const char*, int> check[] = { { "zero", 0 }, { "one", 1 }, { "two", 2 } };

	checkContents(EXTRA_ARGS, check);
}

TEST_F(MapFixture, Collection_map_write_existing)
{
	collection["one"] = 7;

	const CheckValue<const char*, int> check[] = { { "zero", 0 }, { "one", 7 }, { "two", 2 } };

	checkContents(EXTRA_ARGS, check);
}

TEST_F(MapFixture, Collection_map_write_existing_iterator)
{
	auto it = collection.find("one");
	CHECK(it != collection.end());
	*it = 7;

	CheckValue<const char*, int> check[] = { { "zero", 0 }, { "one", 7 }, { "two", 2 } };

	checkContents(EXTRA_ARGS, check);

	it.setValue(42);
	check[1].value = 42;

	checkContents(EXTRA_ARGS, check);
}

TEST_F(MapFixture, Collection_map_write_new)
{
	collection["three"] = 7;

	const CheckValue<const char*, int> check[] = { { "zero", 0 }, { "one", 1 }, { "two", 2 }, { "three", 7 } };

	checkContents(EXTRA_ARGS, check);
}

TEST_F(MapFixture, Collection_map_insert_new)
{
	auto it = collection.insert("three");
	CHECK(it != collection.end());
	*it = -1;

	const CheckValue<const char*, int> check[] = { { "zero", 0 }, { "one", 1 }, { "two", 2 }, { "three", -1 } };

	checkContents(EXTRA_ARGS, check);
}

TEST_F(MapFixture, Collection_map_insert_existing)
{
	auto it = collection.insert("one");
	CHECK(it == collection.end());

	const CheckValue<const char*, int> check[] = { { "zero", 0 }, { "one", 1 }, { "two", 2 } };

	checkContents(EXTRA_ARGS, check);
}

TEST_F(MapFixture, Collection_map_erase_iterator)
{
	auto it = collection.find("one");
	CHECK(it != collection.end());
	collection.erase(it);

	CHECK(collection.find("one") == collection.end());

	const CheckValue<const char*, int> check[] = { { "zero", 0 }, { "two", 2 } };

	checkContents(EXTRA_ARGS, check);
}

TEST_F(MapFixture, Collection_map_erase_key)
{
	CHECK(collection.eraseKey("one") == 1);
	CHECK(collection.find("one") == collection.end());

	const CheckValue<const char*, int> check[] = { { "zero", 0 }, { "two", 2 } };

	checkContents(EXTRA_ARGS, check);
}

TEST_F(MapFixture, Collection_map_const)
{
	const CheckValue<const char*, int> check[] = { { "zero", 0 }, { "one", 1 }, { "two", 2 } };

	checkContents(EXTRA_ARGS, check);

	constCollection["one"] = 7;
	checkContents(EXTRA_ARGS, check);

	auto it = constCollection.find("one");
	CHECK(it != constCollection.end());
	*it = 7;
	checkContents(EXTRA_ARGS, check);

	it.setValue(7);
	checkContents(EXTRA_ARGS, check);

	constCollection["three"] = 7;
	checkContents(EXTRA_ARGS, check);

	CHECK(constCollection.insert("one") == constCollection.end());
	checkContents(EXTRA_ARGS, check);

	CHECK(constCollection.insert("test") == constCollection.end());
	checkContents(EXTRA_ARGS, check);

	CHECK(constCollection.erase(it) == constCollection.end());
	checkContents(EXTRA_ARGS, check);

	CHECK(constCollection.eraseKey("one") == 0);
	checkContents(EXTRA_ARGS, check);
}

TEST_F(MapFixture, Collection_map_iterator)
{
	{
		auto it = constCollection.cbegin();
		CHECK(it == constCollection.cbegin());
		CHECK(it != constCollection.cend());

		++it;
		CHECK(it != constCollection.cbegin());
		CHECK(it != constCollection.cend());

		++it;
		CHECK(it != constCollection.cbegin());
		CHECK(it != constCollection.cend());

		++it;
		CHECK(it != constCollection.cbegin());
		CHECK(it == constCollection.cend());
	}
}

////////////////////////////////////////////////////////////////////////////////

TEST(Collection_multimap)
{
	std::multimap<std::string, int> container;
	container.emplace("one", 1);
	container.emplace("one", 11);
	container.emplace("two", 2);
	container.emplace("zero", 0);

	Collection collection(container);

	iteratorSanityCheck(EXTRA_ARGS, collection);

	// insert
	auto it = collection.insert("two");
	CHECK(it != collection.end());
	*it = 22;

	const CheckValue<const char*, int> check1[] = {
		{ "one", 1 }, { "one", 11 }, { "two", 22 }, { "two", 2 }, { "zero", 0 }
	};

	collectionCheckOrdered(EXTRA_ARGS, collection, check1);

	// erase iterator
	collection.erase(it);

	const CheckValue<const char*, int> check2[] = { { "one", 1 }, { "one", 11 }, { "two", 2 }, { "zero", 0 } };

	collectionCheckOrdered(EXTRA_ARGS, collection, check2);

	// erase key
	CHECK(collection.eraseKey("one") == 2);

	const CheckValue<const char*, int> check3[] = { { "two", 2 }, { "zero", 0 } };

	collectionCheckOrdered(EXTRA_ARGS, collection, check3);
}

TEST(Collection_nested)
{
	std::map<std::string, std::vector<int>> container;
	container["one"].emplace_back(1);
	container["two"].emplace_back(2);
	container["two"].emplace_back(22);

	Collection collection(container);

	CHECK_EQUAL(2, collection.size());

	{
		Variant v = collection["one"];
		Collection c;
		CHECK(v.tryCast(c));

		const CheckValue<size_t, int> check[] = { { 0, 1 } };

		collectionCheck(EXTRA_ARGS, c, check);
	}

	{
		Collection c;
		CHECK(collection["two"].tryCast(c));

		const CheckValue<size_t, int> check[] = { { 0, 2 }, { 1, 22 } };

		collectionCheck(EXTRA_ARGS, c, check);
	}
}

TEST(Collection_downcast)
{
	typedef std::vector<int> VectorInt;

	VectorInt fixture;
	fixture.push_back(1);
	fixture.push_back(7);
	fixture.push_back(42);

	Variant v(fixture);

	VectorInt test;
	CHECK(v.tryCast(test));
	CHECK(test == fixture);
}

TEST(Collection_linear_notifications)
{
	std::vector<int> fixture;
	fixture.push_back(10);
	fixture.push_back(11);
	fixture.push_back(12);

	Collection c(fixture);
	size_t counter = 0;

	c.connectPreErase([&](Collection::Iterator pos, size_t count) {
		CHECK_EQUAL(0, counter);
		counter += 1;

		CHECK_EQUAL(3, c.size());
		CHECK_EQUAL(1, count);
		CHECK_EQUAL(2, pos.key().cast<size_t>());
		CHECK_EQUAL(12, pos.value().cast<int>());
	});

	c.connectPostErased([&](Collection::Iterator pos, size_t count) {
		CHECK_EQUAL(1, counter);
		counter += 1;

		CHECK_EQUAL(1, count);
		CHECK_EQUAL(2, c.size());
		CHECK(c.end() == pos);
	});

	c.connectPreInsert([&](Collection::Iterator pos, size_t count) {
		CHECK_EQUAL(2, counter);
		counter += 1;

		CHECK_EQUAL(1, count);
		CHECK_EQUAL(1, pos.key().cast<size_t>());
		CHECK_EQUAL(11, pos.value().cast<int>());
	});

	c.connectPostInserted([&](Collection::Iterator pos, size_t count) {
		CHECK_EQUAL(3, counter);
		counter += 1;

		CHECK_EQUAL(1, count);
		CHECK_EQUAL(1, pos.key().cast<size_t>());
		CHECK_EQUAL(0, pos.value().cast<int>());
	});

	c.connectPreChange([&](Collection::Iterator pos, const Variant& newValue) {
		CHECK_EQUAL(4, counter);
		counter += 1;

		CHECK_EQUAL(21, newValue.cast<int>());
		CHECK_EQUAL(1, pos.key().cast<size_t>());
		CHECK_EQUAL(0, pos.value().cast<int>());
	});

	c.connectPostChanged([&](Collection::Iterator pos, const Variant& oldValue) {
		CHECK_EQUAL(5, counter);
		counter += 1;

		CHECK_EQUAL(0, oldValue.cast<int>());
		CHECK_EQUAL(1, pos.key().cast<size_t>());
		CHECK_EQUAL(21, pos.value().cast<int>());
	});

	CHECK_EQUAL(0, counter);

	c.eraseKey(2);
	CHECK_EQUAL(2, counter);

	auto it = c.insert(1);
	CHECK_EQUAL(4, counter);
	CHECK(it.key() == 1);
	CHECK(it.value() == 0);

	it.setValue(21);
	CHECK_EQUAL(6, counter);
	CHECK(it.value() == 21);
}

TEST(Collection_mapping_notifications)
{
	std::map<std::string, int> fixture;
	fixture["0 zero"] = 0;
	fixture["1 one"] = 1;
	fixture["2 two"] = 2;

	Collection c(fixture);
	size_t counter = 0;

	c.connectPreErase([&](Collection::Iterator pos, size_t count) {
		CHECK_EQUAL(0, counter);
		counter += 1;

		CHECK_EQUAL(3, c.size());
		CHECK_EQUAL(1, count);
		CHECK_EQUAL("1 one", pos.key().cast<const std::string&>());
		CHECK_EQUAL(1, pos.value().cast<int>());
	});

	c.connectPostErased([&](Collection::Iterator pos, size_t count) {
		CHECK_EQUAL(1, counter);
		counter += 1;

		CHECK_EQUAL(2, c.size());
		CHECK_EQUAL(1, count);

		CHECK_EQUAL("2 two", pos.key().cast<const std::string&>());
		CHECK_EQUAL(2, pos.value().cast<int>());
	});

	c.connectPreInsert([&](Collection::Iterator pos, size_t count) {
		CHECK_EQUAL(2, counter);
		counter += 1;

		CHECK_EQUAL(1, count);
		CHECK_EQUAL("2 two", pos.key().cast<const std::string&>());
		CHECK_EQUAL(2, pos.value().cast<int>());
	});

	c.connectPostInserted([&](Collection::Iterator pos, size_t count) {
		CHECK_EQUAL(3, counter);
		counter += 1;

		CHECK_EQUAL(1, count);
		CHECK_EQUAL("1 uno", pos.key().cast<const std::string&>());
		CHECK_EQUAL(0, pos.value().cast<int>());
	});

	c.connectPreChange([&](Collection::Iterator pos, const Variant& newValue) {
		CHECK_EQUAL(4, counter);
		counter += 1;

		CHECK_EQUAL(1, newValue.cast<int>());
		CHECK_EQUAL("1 uno", pos.key().cast<const std::string&>());
		CHECK_EQUAL(0, pos.value().cast<int>());
	});

	c.connectPostChanged([&](Collection::Iterator pos, const Variant& oldValue) {
		CHECK_EQUAL(5, counter);
		counter += 1;

		CHECK_EQUAL(0, oldValue.cast<int>());
		CHECK_EQUAL("1 uno", pos.key().cast<const std::string&>());
		CHECK_EQUAL(1, pos.value().cast<int>());
	});

	CHECK_EQUAL(0, counter);

	c.eraseKey("1 one");
	CHECK_EQUAL(2, counter);

	auto it = c.insert("1 uno");
	CHECK_EQUAL(4, counter);
	CHECK(it.key() == "1 uno");
	CHECK(it.value() == 0);

	it.setValue(1);
	CHECK_EQUAL(6, counter);
	CHECK(it.value() == 1);
}
} // end namespace wgt
