#include "pch.hpp"

#include "core_reflection/interfaces/i_class_definition.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/definition_manager.hpp"

#include "test_objects.hpp"

namespace wgt
{


TestDefinitionFixture::TestDefinitionFixture()
{
	IDefinitionManager& definitionManager = getDefinitionManager();
	klass_ = definitionManager.getDefinition<TestDefinitionObject>();
	assert(klass_);
	derived_klass_ = definitionManager.getDefinition<TestDefinitionDerivedObject>();
	assert(derived_klass_);
}

namespace
{
uint32_t RandomNumber32()
{
	RefObjectId uid(RefObjectId::generate());
	return uid.getA() + uid.getB() + uid.getC() + uid.getD();
}

uint64_t RandomNumber64()
{
	RefObjectId uid(RefObjectId::generate());
	uint64_t a = (uint64_t)uid.getA();
	uint64_t b = (uint64_t)uid.getB();
	uint64_t c = (uint64_t)uid.getC();
	uint64_t d = (uint64_t)uid.getD();
	return (a << 32) + (b << 32) + c + d;
}

std::string RandomString()
{
	std::string random("Random Data: ");
	random += RefObjectId::generate().toString();
	random += RefObjectId::generate().toString();
	random += RefObjectId::generate().toString();
	random += RefObjectId::generate().toString();
	return random;
}
}

//------------------------------------------------------------------------------
TestDefinitionObject::TestDefinitionObject()
    : counter_(0), raw_string_("RAW READ-0NLY STR1NG!!!"), raw_wstring_(L"RAW READ-0NLY W1DE STR1NG!!!"),
      boolean_(false), uint32_(0), int32_(0), uint64_(0L), float_(0.f)
{
	strings_.push_back("string1");
	strings_.push_back("string2");
}

//------------------------------------------------------------------------------
bool TestDefinitionObject::operator==(const TestDefinitionObject& tdo) const
{
	if (counter_ != tdo.counter_)
		return false;

	if (text_ != tdo.text_)
		return false;

	if (std::string(raw_string_) != std::string(tdo.raw_string_))
		return false;

	if (string_ != tdo.string_ || strings_ != tdo.strings_)
		return false;

	if (std::wstring(raw_wstring_) != std::wstring(tdo.raw_wstring_))
		return false;

	if (wstring_ != tdo.wstring_ || wstrings_ != tdo.wstrings_)
		return false;

	if (exposedStruct_ != tdo.exposedStruct_ || exposedStructs_ != tdo.exposedStructs_)
		return false;

	if (!(exposedObject_ == tdo.exposedObject_ && exposedObjects_ == tdo.exposedObjects_))
		return false;

	if (boolean_ != tdo.boolean_ || booleans_ != tdo.booleans_)
		return false;

	if (uint32_ != tdo.uint32_ || uint32s_ != tdo.uint32s_)
		return false;

	if (int32_ != tdo.int32_ || int32s_ != tdo.int32s_)
		return false;

	if (uint64_ != tdo.uint64_ || uint64s_ != tdo.uint64s_)
		return false;

	if (float_ != tdo.float_ || floats_ != tdo.floats_)
		return false;

	if (vector3_ != tdo.vector3_ || vector3s_ != tdo.vector3s_)
		return false;

	if (vector4_ != tdo.vector4_ || vector4s_ != tdo.vector4s_)
		return false;

	if (binary_->compare(*tdo.binary_) != 0 || binaries_.size() != tdo.binaries_.size())
		return false;

	auto i = 0u;
	for (; i < binaries_.size() && binaries_[i]->compare(*tdo.binaries_[i]) == 0; ++i)
		;
	if (i != binaries_.size())
		return false;

	if (multidimensional_ != tdo.multidimensional_)
		return false;

	return true;
}

//------------------------------------------------------------------------------
bool TestDefinitionObject::operator!=(const TestDefinitionObject& tdo) const
{
	return !operator==(tdo);
}
} // end namespace wgt
