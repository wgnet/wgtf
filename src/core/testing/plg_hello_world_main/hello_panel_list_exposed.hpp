
#pragma once
#ifndef _HELLO_PANEL_LIST_EXPOSED_HPP
#define _HELLO_PANEL_LIST_EXPOSED_HPP

#include "core_reflection/ref_object_id.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_variant/collection.hpp"
#include <vector>
#include <string>

namespace wgt
{
class HelloPanelListExposed
{
	DECLARE_REFLECTED

public:
	HelloPanelListExposed() : sampleCollection_(sampleList_)
	{
		// Fill the container with random strings
		const int itemCount = 100;
		sampleList_.resize(itemCount);
		std::generate(sampleList_.begin(), sampleList_.end(), []() { return RefObjectId::generate().toString(); });
	}

	std::string getCollectionItem(int index)
	{
		if (index >= 0 && index < static_cast<int>(sampleList_.size()))
		{
			return sampleList_[index];
		}
		return "-";
	}

private:
	Collection sampleCollection_;
	std::vector<std::string> sampleList_;
};
} // end namespace wgt

#endif // _HELLO_PANEL_LIST_EXPOSED_HPP