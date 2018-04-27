#pragma once

#include <vector>

namespace wgt
{
struct ComponentVersion
{
	static std::vector<int> tokenise(const char* version);
	bool operator()(const std::vector<int>& a, const std::vector<int>& b) const;
};
}
