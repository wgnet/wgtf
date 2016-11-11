#include "meta_type.hpp"
#include "variant.hpp"
#include "wg_types/string_ref.hpp"
#include "core_serialization/fixed_memory_stream.hpp"
#include "core_serialization/resizing_memory_stream.hpp"
#include "core_serialization/text_stream.hpp"

#include <cstring>
#include <cassert>
#include <cstdint>
#include <unordered_map>
#include <memory>

namespace wgt
{
namespace
{
struct MetaTypeIndex
{
	std::unordered_map<wgt::StringRef, const MetaType*> nameIndex_;
	std::unordered_map<const TypeId, const MetaType*> typeIdIndex_;
};

DLink& s_metaTypes()
{
	static DLink inst;
	return inst;
}

std::unique_ptr<MetaTypeIndex> s_index;

bool convertFromString(const MetaType* toType, void* to, const MetaType* fromType, const void* from)
{
	if (fromType->typeId() != TypeId::getType<std::string>())
	{
		return false;
	}

	if (!to || !from)
	{
		return true;
	}

	const std::string& fromStr = *reinterpret_cast<const std::string*>(from);
	FixedMemoryStream dataStream(fromStr.c_str(), fromStr.size());
	TextStream stream(dataStream);
	toType->streamIn(stream, to);
	return !stream.fail() && // conversion succeeded
	stream.peek() == EOF; // whole string was consumed
}
}

MetaType::MetaType(const char* name, const Data& data)
    : data_(data), name_(name ? name : data.typeId_.getName()), link_()
{
	for (int i = 0; i <= QualifiersMask; ++i)
	{
		// Qualified instance must be at least 4 bytes aligned (Variant uses 2 lower bits for storage type)
		assert((reinterpret_cast<uintptr_t>(qualified_ + i) & 0x03) == 0);
		qualified_[i].type_ = this;
	}

	s_metaTypes().prepend(&link_);
	s_index.reset();
}

MetaType::~MetaType()
{
	s_index.reset();
}

const MetaType::Qualified* MetaType::qualified(int qualifiers) const
{
	assert(qualifiers >= 0);
	assert(qualifiers < QualifiersMask);

	return qualified_ + qualifiers;
}

bool MetaType::convertFrom(void* to, const MetaType* fromType, const void* from) const
{
	if (fromType == this)
	{
		// identity conversion
		copy(to, from);
		return true;
	}

	if (*fromType == *this)
	{
		// identity conversion
		copy(to, from);
		return true;
	}

	return convertFromString(this, to, fromType, from);
}

bool MetaType::canConvertFrom(const MetaType* fromType) const
{
	if (fromType == this)
	{
		// identity conversion
		return true;
	}

	if (*fromType == *this)
	{
		// identity conversion
		return true;
	}

	return convertFrom(nullptr, fromType, nullptr);
}

bool MetaType::convertTo(const MetaType* toType, void* to, const void* from) const
{
	return toType->convertFrom(to, this, from);
}

bool MetaType::canConvertTo(const MetaType* toType) const
{
	return toType->canConvertFrom(this);
}

bool MetaType::castPtr(const TypeId& destType, void** dest, void* src) const
{
	if (destType != data_.typeId_)
	{
		return false;
	}

	if (dest)
	{
		*dest = src;
	}

	return true;
}

bool MetaType::operator==(const MetaType& other) const
{
	return data_.typeId_ == other.data_.typeId_ && std::strcmp(name_, other.name_) == 0;
}

const MetaType* MetaType::find(const char* name)
{
	validateIndex();
	// support old data format
	if (strcmp("blob", name) == 0)
	{
		name = "BinaryBlock";
	}
	else if (strcmp("vector2", name) == 0)
	{
		name = "Vector2";
	}
	else if (strcmp("vector3", name) == 0)
	{
		name = "Vector3";
	}
	else if (strcmp("vector4", name) == 0)
	{
		name = "Vector4";
	}
	else if (strcmp("collection", name) == 0)
	{
		name = "Collection";
	}
	auto it = s_index->nameIndex_.find(name);
	if (it != s_index->nameIndex_.end())
	{
		return it->second;
	}

	return nullptr;
}

const MetaType* MetaType::find(const TypeId& typeId)
{
	validateIndex();

	auto it = s_index->typeIdIndex_.find(typeId);
	if (it != s_index->typeIdIndex_.end())
	{
		return it->second;
	}

	return nullptr;
}

void MetaType::validateIndex()
{
	if (!s_index)
	{
		auto index = new MetaTypeIndex();
		for (auto link = s_metaTypes().next(); link != &s_metaTypes(); link = link->next())
		{
			auto metaType = dlink_holder(MetaType, link_, link);
			index->nameIndex_.emplace(metaType->name(), metaType);
			index->typeIdIndex_.emplace(metaType->typeId(), metaType);
		}
		s_index.reset(index);
	}
}

} // end namespace wgt