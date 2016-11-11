#include "layout_hint.hpp"
#include "layout_tags.hpp"

#include <algorithm>
#include <vector>

namespace wgt
{
LayoutHint::LayoutHint()
{
}

LayoutHint::LayoutHint(const char* hint, float strength)
{
	hints_.insert(std::make_pair(std::string(hint), strength));
}

LayoutHint LayoutHint::operator+(const LayoutHint& other)
{
	auto result = *this;
	return result += other;
}

LayoutHint& LayoutHint::operator+=(const LayoutHint& other)
{
	for (auto& hint : other.hints_)
	{
		auto it = hints_.find(hint.first);
		if (it != hints_.end())
		{
			it->second = (it->second + hint.second) / 2.0f;
		}
		else
		{
			hints_.insert(hint);
		}
	}
	return *this;
}

void LayoutHint::clear()
{
	hints_.clear();
}

float LayoutHint::match(const LayoutTags& tags) const
{
	float total = 0.f;
	float matched = 0.f;

	auto icmp = [](const char& c1, const char& c2) -> bool { return tolower(c1) == tolower(c2); };

	for (auto& hint : hints_)
	{
		total += hint.second;
		if (std::find_if(tags.tags_.begin(), tags.tags_.end(), [&hint, &icmp](const std::string& tag) -> bool {
// https://msdn.microsoft.com/en-us/library/4h8ef82f.aspx
// "Use the dual-range overloads in C++14 code because the overloads that only
// take a single iterator for the second range will not detect differences if
// the second range is longer than the first range, and will result in
// undefined behavior if the second range is shorter than the first range.
#if (_MSC_VER >= 1900) // VS2015
			    return std::equal(hint.first.cbegin(), hint.first.cend(), tag.cbegin(), tag.cend(), icmp);
#else // (_MSC_VER >= 1900)
			                 return std::equal(hint.first.cbegin(),
			                                   hint.first.cend(),
			                                   tag.cbegin(),
			                                   icmp);
#endif // (_MSC_VER >= 1900)
			}) != tags.tags_.end())
		{
			matched += hint.second;
		}
	}
	if (total > 0.f)
	{
		return matched / total;
	}
	return 0.f;
}

const std::map<std::string, float>& LayoutHint::hints() const
{
	return hints_;
}
} // end namespace wgt
