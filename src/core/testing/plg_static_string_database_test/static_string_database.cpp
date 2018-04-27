#include "static_string_database.hpp"

namespace wgt
{

bool StaticStringDatabase::hasId(uint64_t id) const
{
	return id < m_strings.size();
}

uint64_t StaticStringDatabase::find(const char* string) const
{
	auto it = m_keys.find(string);
	return it == m_keys.end() ? npos() : it->second;
}

uint64_t StaticStringDatabase::insert(const char* string)
{
	auto it = m_keys.find(string);
	if (it != m_keys.end())
	{
		return it->second;
	}
	auto id = m_strings.size();
	m_strings.push_back(string);
	m_keys[string] = id;
	return id;
}

const Collection StaticStringDatabase::idToStringMapping() const
{
	return Collection(m_strings);
}

const char* StaticStringDatabase::getText(uint64_t id) const
{
	return id < m_strings.size() ? m_strings.at(static_cast<size_t>(id)).c_str() : nullptr;
}

}
