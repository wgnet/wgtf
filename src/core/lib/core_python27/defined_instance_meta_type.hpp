#pragma once

#include "core_variant/meta_type.hpp"
#include "defined_instance.hpp"

namespace wgt
{

namespace ReflectedPython
{

class DefinedInstanceMetaType:
	public MetaType
{
	typedef MetaType base;
	typedef ReflectedPython::DefinedInstance value_type;

public:
	DefinedInstanceMetaType();

	void init(void* value) const override;
	void copy(void* dest, const void* src) const override;
	void move(void* dest, void* src) const override;
	void destroy(void* value) const override;
	bool equal(const void* lhs, const void* rhs) const override;

	void streamOut(TextStream& stream, const void* value) const override;
	void streamIn(TextStream& stream, void* value) const override;

	void streamOut(BinaryStream& stream, const void* value) const override;
	void streamIn(BinaryStream& stream, void* value) const override;

protected:
	static value_type& cast(void* value)
	{
		return *static_cast<value_type*>(value);
	}

	static const value_type& cast(const void* value)
	{
		return *static_cast<const value_type*>(value);
	}

};

}

}
