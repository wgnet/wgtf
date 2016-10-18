#include "pch.hpp"

#include "core_data_model/variant_list.hpp"
#include "core_unit_test/unit_test.hpp"

namespace wgt
{
bool isListValid(const VariantList& list, size_t expectedSize)
{
	int valueCounter = 0;

	if (list.size() != expectedSize)
	{
		return false;
	}

	for (size_t i = 0; i < list.size(); ++i)
	{
		if (!list.item(i))
		{
			return false;
		}

		const auto value = list.item(i)->getData(0, ValueRole::roleId_);
		if (valueCounter != value.cast<int>())
		{
			return false;
		}
		++valueCounter;
	}

	return true;
}

TEST(testVariantListInsert)
{
	{ // Test inserting a larger list into the end of a smaller one
		VariantList smallList;
		VariantList largeList;
		smallList.push_back(Variant(0));
		largeList.push_back(Variant(1));
		largeList.push_back(Variant(2));
		const auto expectedSize = smallList.size() + largeList.size();
		smallList.insert(smallList.end(), largeList.begin(), largeList.end());
		CHECK(isListValid(smallList, expectedSize));
	}

	{ // Test inserting a larger list into the start of a smaller one
		VariantList smallList;
		VariantList largeList;
		smallList.push_back(Variant(2));
		largeList.push_back(Variant(0));
		largeList.push_back(Variant(1));
		const auto expectedSize = smallList.size() + largeList.size();
		smallList.insert(smallList.begin(), largeList.begin(), largeList.end());
		CHECK(isListValid(smallList, expectedSize));
	}

	{ // Test inserting a larger list into the middle of a smaller one
		VariantList smallList;
		VariantList largeList;
		smallList.push_back(Variant(0));
		smallList.push_back(Variant(5));
		smallList.push_back(Variant(6));
		largeList.push_back(Variant(1));
		largeList.push_back(Variant(2));
		largeList.push_back(Variant(3));
		largeList.push_back(Variant(4));
		const auto expectedSize = smallList.size() + largeList.size();
		smallList.insert(smallList.begin() + (smallList.size() / 2), largeList.begin(), largeList.end());
		CHECK(isListValid(smallList, expectedSize));
	}

	{ // Test inserting a smaller list into the end of a larger one
		VariantList smallList;
		VariantList largeList;
		smallList.push_back(Variant(2));
		largeList.push_back(Variant(0));
		largeList.push_back(Variant(1));
		const auto expectedSize = smallList.size() + largeList.size();
		largeList.insert(largeList.end(), smallList.begin(), smallList.end());
		CHECK(isListValid(largeList, expectedSize));
	}

	{ // Test inserting a smaller list into the start of a larger one
		VariantList smallList;
		VariantList largeList;
		smallList.push_back(Variant(0));
		largeList.push_back(Variant(1));
		largeList.push_back(Variant(2));
		const auto expectedSize = smallList.size() + largeList.size();
		largeList.insert(largeList.begin(), smallList.begin(), smallList.end());
		CHECK(isListValid(largeList, expectedSize));
	}

	{ // Test inserting a smaller list into the middle of a larger one
		VariantList smallList;
		VariantList largeList;
		smallList.push_back(Variant(1));
		largeList.push_back(Variant(0));
		largeList.push_back(Variant(2));
		largeList.push_back(Variant(3));
		const auto expectedSize = smallList.size() + largeList.size();
		largeList.insert(largeList.begin() + (largeList.size() / 2), smallList.begin(), smallList.end());
		CHECK(isListValid(largeList, expectedSize));
	}
}

} // end namespace wgt
