#include "pch.hpp"

#include "CppUnitLite2/src/CppUnitLite2.h"
#include "core_serialization/resizing_memory_stream.hpp"
#include "core_serialization/text_stream.hpp"
#include "core_serialization/text_stream_manip.hpp"
#include "core_serialization_xml/xml_serializer.hpp"
#include "core_variant/variant.hpp"
#include "core_variant/collection.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/function_property.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/utilities/reflection_function_utilities.hpp"
#include "core_unit_test/test_framework.hpp"
#include "core_object/managed_object.hpp"
#include <string>
#include <vector>
#include <map>
#include <list>
#include <utility>
#include <memory>
#include <cmath>
#include <limits>

namespace wgt
{
IDefinitionManager& definitionManager();

template <typename T>
const IClassDefinition* classDefinition()
{
	static const IClassDefinition* s_classDefinitionPtr = nullptr;
	if (!s_classDefinitionPtr)
	{
		s_classDefinitionPtr = definitionManager().registerDefinition<TypeClassDefinition<T>>();
	}

	return s_classDefinitionPtr;
}

template <typename T>
ManagedObject<T> createObject()
{
	return ManagedObject<T>(std::make_unique<T>());
}

template <typename T, typename Arg0>
ManagedObject<T> createObject(Arg0&& arg0)
{
	return ManagedObject<T>(std::make_unique<T>(std::forward<Arg0>(arg0)));
}

class SimpleTestObject
{
	DECLARE_REFLECTED
	friend TextStream& operator<<(TextStream& stream, const SimpleTestObject& value);
	friend TextStream& operator>>(TextStream& stream, SimpleTestObject& value);

public:
	explicit SimpleTestObject(std::string s = "hello", int i = 42, double d = 1.23456789)
	    : s_(std::move(s)), i_(i), d_(d)
	{
	}

	bool operator<(const SimpleTestObject& that) const
	{
		if (s_ < that.s_)
		{
			return true;
		}

		if (i_ < that.i_)
		{
			return true;
		}

		if (d_ < that.d_)
		{
			return true;
		}

		return false;
	}

	bool operator==(const SimpleTestObject& that) const
	{
		return s_ == that.s_ && i_ == that.i_ && std::abs(d_ - that.d_) < 0.0001;
	}

	bool operator!=(const SimpleTestObject& that) const
	{
		return !(*this == that);
	}

private:
	std::string s_;
	int i_;
	double d_;
};

TextStream& operator<<(TextStream& stream, const SimpleTestObject& value)
{
	stream << quoted(value.s_) << " " << value.i_ << " " << value.d_;
	return stream;
}

TextStream& operator>>(TextStream& stream, SimpleTestObject& value)
{
	stream >> quoted(value.s_) >> value.i_ >> value.d_;
	return stream;
}

class ComplexTestObject
{
	DECLARE_REFLECTED

public:
	ComplexTestObject()
	{
	}

	void init(const ManagedObject<SimpleTestObject>& linkTo)
	{
		linkedObject_ = linkTo.getHandle();
		object_ = SimpleTestObject("obj_");
		string_ = "ololo";

		objectVector_.clear();
		objectVector_.push_back(SimpleTestObject("object 0"));
		objectVector_.push_back(SimpleTestObject("object 1"));
		objectVector_.push_back(SimpleTestObject("object 2"));

		stringIntMap_.clear();
		stringIntMap_["0 zero"] = 0;
		stringIntMap_["1 one"] = 1;
		stringIntMap_["2 two"] = 2;

		stringObjectMapVector_.clear();

		std::map<std::string, SimpleTestObject> map;
		map.emplace("obj0", SimpleTestObject("value 0.0"));
		map.emplace("obj1", SimpleTestObject("value 0.1"));
		map.emplace("obj2", SimpleTestObject("value 0.2"));
		stringObjectMapVector_.push_back(std::move(map));

		map.emplace("obj0", SimpleTestObject("value 1.0"));
		map.emplace("obj1", SimpleTestObject("value 1.1"));
		map.emplace("obj2", SimpleTestObject("value 1.2"));
		stringObjectMapVector_.push_back(std::move(map));
	}

	bool operator==(const ComplexTestObject& that) const
	{
		if (object_ != that.object_ || string_ != that.string_ || objectVector_ != that.objectVector_ || stringIntMap_ != that.stringIntMap_)
		{
			return false;
		}

		if (stringObjectMapVector_.size() != that.stringObjectMapVector_.size())
		{
			return false;
		}

		if (linkedObject_.getBase<SimpleTestObject>() != that.linkedObject_.getBase<SimpleTestObject>())
		{
			return false;
		}

		// vi = vector iterator, tvi = that vector iterator
		for (auto vi = stringObjectMapVector_.begin(), tvi = that.stringObjectMapVector_.begin(); vi != stringObjectMapVector_.end(); ++vi, ++tvi)
		{
			if (vi->size() != tvi->size())
			{
				return false;
			}

			// mi = map iterator, tmi = that map iterator
			for (auto mi = vi->begin(), tmi = tvi->begin(); mi != vi->end(); ++mi, ++tmi)
			{
				if (mi->first != tmi->first)
				{
					return false;
				}

				auto* p = &mi->second;
				auto* tp = &tmi->second;

				if (!p || !tp || *p != *tp)
				{
					return false;
				}
			}
		}

		return true;
	}

	bool operator!=(const ComplexTestObject& that) const
	{
		return !(*this == that);
	}

	ObjectHandle linkedObject_;
	SimpleTestObject object_;

	std::string string_;
	std::vector<SimpleTestObject> objectVector_;
	std::map<std::string, int> stringIntMap_;
	std::vector<std::map<std::string, SimpleTestObject>> stringObjectMapVector_;
};

IDefinitionManager& definitionManager()
{
	static std::unique_ptr<TestFramework> s_framework = nullptr;
	if (!s_framework)
	{
        s_framework = std::make_unique<TestFramework>();
	}
	return s_framework->getDefinitionManager();
}

BEGIN_EXPOSE(SimpleTestObject, MetaNone())
EXPOSE("string", s_)
EXPOSE("integer", i_)
EXPOSE("real", d_)
END_EXPOSE()

BEGIN_EXPOSE(ComplexTestObject, MetaNone())
EXPOSE("object", object_)
EXPOSE("link", linkedObject_)
EXPOSE("string", string_)
EXPOSE("objects", objectVector_)
EXPOSE("map_string_to_int", stringIntMap_)
EXPOSE("maps_string_to_obj", stringObjectMapVector_)
END_EXPOSE()

TEST(XMLSerializer_simple)
{
	ResizingMemoryStream dataStream;
	XMLSerializer serializer(dataStream, definitionManager());

	Variant src = 42;
	CHECK(serializer.serialize(src));
	CHECK(serializer.serialize("hello"));

	CHECK(serializer.sync());
	CHECK_EQUAL(0, dataStream.seek(0));

	Variant dst;
	CHECK(serializer.deserialize(dst));
	CHECK(dst.canCast<int>());
	CHECK(dst == 42);

	std::string s;
	CHECK(serializer.deserialize(s));
	CHECK(s == "hello");
}

TEST(XMLSerializer_void)
{
	ResizingMemoryStream dataStream;
	XMLSerializer serializer(dataStream, definitionManager());

	Variant voidSource(MetaType::get<void>());
	CHECK(voidSource.isVoid());
	CHECK(serializer.serialize(voidSource));

	CHECK(serializer.sync());
	CHECK_EQUAL(0, dataStream.seek(0));

	Variant voidDest;
	CHECK(serializer.deserialize(voidDest));

	CHECK(voidDest.isVoid());
	CHECK(voidDest == voidSource);
}

TEST(XMLSerializer_reflected)
{
	ResizingMemoryStream dataStream;
	XMLSerializer serializer(dataStream, definitionManager());
	// serializer.setFormat( XMLSerializer::Format::Unformatted() );
	classDefinition<SimpleTestObject>();
	classDefinition<ComplexTestObject>();
	auto linkToObject_ = createObject<SimpleTestObject>("default");
	auto srcObject = createObject<ComplexTestObject>();
	auto srcObjectHandle = srcObject.getHandleT();
	srcObjectHandle->init(linkToObject_);
	CHECK(serializer.serialize(srcObjectHandle));
	auto srcData = *srcObjectHandle.get();
	CHECK(serializer.sync());
	CHECK_EQUAL(0, dataStream.seek(0));
	srcObject = nullptr;

	ManagedObject<ComplexTestObject> dstObject;
	auto dstObjectHandle = dstObject.getHandleT();
	CHECK(dstObjectHandle.get() == nullptr);
	CHECK(serializer.deserialize(dstObject));
	dstObjectHandle = dstObject.getHandleT();
	ComplexTestObject* dstTestObject = dstObjectHandle.get();
	RETURN_ON_FAIL_CHECK(dstTestObject != nullptr);

	CHECK(*dstTestObject == srcData);

	auto tmpTestObject = createObject<ComplexTestObject>();
	auto tmpTestObjectHandle = tmpTestObject.getHandleT();
	CHECK(*dstTestObject != *tmpTestObject.getHandleT());

	tmpTestObjectHandle->init(linkToObject_);
	CHECK(*dstTestObject == *tmpTestObjectHandle);

	tmpTestObjectHandle->stringObjectMapVector_[1]["obj1"] = SimpleTestObject("value 1.1 modified");
	CHECK(*dstTestObject != *tmpTestObjectHandle);
}
} // end namespace wgt
META_TYPE(wgt::SimpleTestObject)
