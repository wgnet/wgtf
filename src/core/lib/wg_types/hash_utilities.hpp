#ifndef HASH_UTILITIES_HPP
#define HASH_UTILITIES_HPP

#include <stdint.h>
#include <string>

namespace wgt
{
namespace HashUtilities
{
uint64_t compute(const void* data, size_t length);
uint64_t compute(const char* value);
uint64_t compute(const std::string& value);
uint64_t compute(const wchar_t* value);
uint64_t compute(const std::wstring& value);
uint64_t compute(int);
uint64_t compute(unsigned int value);
uint64_t compute(int64_t value);
uint64_t compute(uint64_t value);

//Case insensitive hash
uint64_t computei(const char* value);

template <typename T>
void combine(uint64_t& seed, const T& value)
{
	seed ^= compute(value) +
	// 2^64/phi.
	0x9E3779B97F4A7C15 +
	// make sure bits spread across the output even if input hashes
	// have a small output range.
	(seed << 5) + (seed >> 3);
}

void directCombine(uint64_t& seed, uint64_t value);
}
} // end namespace wgt
#endif // HASH_UTILITIES_HPP
