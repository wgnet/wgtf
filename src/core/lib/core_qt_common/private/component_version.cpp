#include "component_version.hpp"

#include <algorithm>
#include <cstdlib>

namespace wgt
{
std::vector<int> ComponentVersion::tokenise(const char* version)
{
	static const size_t bufferSize = 256;

	std::vector<int> tokens;

	auto len = strlen(version);
	if (len >= bufferSize)
	{
		return tokens;
	}

	char buffer[bufferSize];
	strcpy(buffer, version);
	auto tok = strtok(buffer, ".");
	while (tok != nullptr)
	{
		tokens.push_back(strtol(tok, nullptr, 0));
		tok = strtok(nullptr, ".");
	}
	return tokens;
}

bool ComponentVersion::operator()(const std::vector<int>& a, const std::vector<int>& b) const
{
	auto count = std::min(a.size(), b.size());
	for (size_t i = 0; i < count; ++i)
	{
		if (a[i] == b[i])
		{
			continue;
		}

		return a[i] < b[i];
	}
	return a.size() < b.size();
}
}
