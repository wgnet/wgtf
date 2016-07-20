#include "pch.hpp"
#include "defined_instance_meta_type.hpp"

namespace wgt
{

namespace ReflectedPython
{

DefinedInstanceMetaType::DefinedInstanceMetaType():
	base("DefinedInstance", data< value_type >())
{
}

void DefinedInstanceMetaType::init(void* value) const
{
	new (value) value_type();
}

void DefinedInstanceMetaType::copy(void* dest, const void* src) const
{
	// should never be called
	assert(false);
}

void DefinedInstanceMetaType::move(void* dest, void* src) const
{
	// should never be called
	assert(false);
}

void DefinedInstanceMetaType::destroy(void* value) const
{
	cast(value).~value_type();
}

bool DefinedInstanceMetaType::equal(const void* lhs, const void* rhs) const
{
	// different instances are always different
	return lhs == rhs;
}

void DefinedInstanceMetaType::streamOut(TextStream& stream, const void* value) const
{
	stream << cast(value);
}

void DefinedInstanceMetaType::streamIn(TextStream& stream, void* value) const
{
	stream.setState( std::ios_base::failbit );
}

void DefinedInstanceMetaType::streamOut(BinaryStream& stream, const void* value) const
{
	stream.setState( std::ios_base::failbit );
}

void DefinedInstanceMetaType::streamIn(BinaryStream& stream, void* value) const
{
	stream.setState( std::ios_base::failbit );
}

}

}
