#include "hash_utilities.hpp"
// TODO: Create multi-platform generic types header
#include "core_common/ngt_windows.hpp"

namespace wgt
{
namespace HashUtilities
{
static const uint64_t FNV_prime = 1099511628211UL;
static const uint64_t FNV_offset_basis = 14695981039346656037UL;

//------------------------------------------------------------------------------
uint64_t compute(const void* data, size_t length)
{
	const char* input = static_cast<const char*>(data);
	uint64_t result = FNV_offset_basis;
	for (size_t i = 0; i < length; ++i)
	{
		result = (result ^ input[i]) * FNV_prime;
	}
	return result;
}

//------------------------------------------------------------------------------
uint64_t compute(const char* value)
{
	const char* input = value;
	uint64_t result = FNV_offset_basis;
	while (*input)
	{
		result = (result ^ *input) * FNV_prime;
		++input;
	}
	return result;
}


//------------------------------------------------------------------------------
uint64_t computei(const char* value )
{
	const char* input = value;
	uint64_t result = FNV_offset_basis;
	while (*input)
	{
		result = (result ^ ::tolower(*input)) * FNV_prime;
		++input;
	}
	return result;
}

//------------------------------------------------------------------------------
uint64_t compute(const wchar_t* value)
{
	size_t strLen = wcslen(value);
	return compute(reinterpret_cast<const char*>(value), strLen * sizeof(wchar_t));
}

//------------------------------------------------------------------------------
uint64_t compute(const std::wstring& value)
{
	return compute(reinterpret_cast<const char*>(value.c_str()), value.length() * sizeof(wchar_t));
}

//------------------------------------------------------------------------------
uint64_t compute(const std::string& value)
{
	return compute(value.c_str(), value.length());
}

//------------------------------------------------------------------------------
uint64_t compute(int value)
{
	return compute(static_cast<int64_t>(value));
}

//------------------------------------------------------------------------------
uint64_t compute(unsigned int value)
{
	return compute(static_cast<uint64_t>(value));
}

//------------------------------------------------------------------------------
uint64_t compute(int64_t value)
{
	return compute((const void*)&value, sizeof(int64_t));
}

//------------------------------------------------------------------------------
uint64_t compute(uint64_t value)
{
	return compute((const void*)&value, sizeof(uint64_t));
}

void directCombine(uint64_t& seed, uint64_t value)
{
	seed ^= value +
	// 2^64/phi.
	0x9E3779B97F4A7C15 +
	// make sure bits spread across the output even if input hashes
	// have a small output range.
	(seed << 5) + (seed >> 3);
}
}
} // end namespace wgt
