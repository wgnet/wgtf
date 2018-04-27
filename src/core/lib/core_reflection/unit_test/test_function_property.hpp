#ifndef TEST_FUNCTION_PROPERTY_HPP
#define TEST_FUNCTION_PROPERTY_HPP

#include "core_common/assert.hpp"
#include "core_reflection/definition_manager.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_unit_test/unit_test.hpp"
#include "core_object/managed_object.hpp"
#include "test_helpers.hpp"
#include "test_reflection_fixture.hpp"
#include "wg_types/binary_block.hpp"

#include <memory>

namespace wgt
{
// =============================================================================
class TestPropertyFixtureBase : public TestReflectionFixture
{
private:
	static TestPropertyFixtureBase* s_instance;

public:
	TestPropertyFixtureBase();

	~TestPropertyFixtureBase()
	{
		TF_ASSERT(s_instance != nullptr);
		s_instance = nullptr;
	}

	static TestPropertyFixtureBase& getFixture()
	{
		TF_ASSERT(s_instance != nullptr);
		return *s_instance;
	}

	typedef std::unique_ptr<BaseProperty> BasePropertyPtr;

	BasePropertyPtr booleanProperty_;
	BasePropertyPtr integerProperty_;
	BasePropertyPtr uintegerProperty_;
	BasePropertyPtr floatProperty_;
	BasePropertyPtr stringProperty_;
	BasePropertyPtr wstringProperty_;
	// BasePropertyPtr rawStringProperty_;
	// BasePropertyPtr rawWStringProperty_;
	BasePropertyPtr binaryDataProperty_;
	BasePropertyPtr exposedStructProperty_;
	BasePropertyPtr exposedObjectProperty_;

	class TestPropertyObject
	{
	public:
        TestPropertyObject()
        {
            exposedObject_ = ManagedObject<ExposedObject>::make();
            exposedHandle_ = exposedObject_.getHandleT();
        }

        struct ExposedStruct
        {
            ExposedStruct() : boolean_(false)
            {
            }
            bool boolean_;

            bool operator==(const ExposedStruct& other) const
            {
                return other.boolean_ == boolean_;
            }
            bool operator!=(const ExposedStruct& other) const
            {
                return !operator==(other);
            }
        };

        struct ExposedObject
        {
            DECLARE_REFLECTED
        public:
            ExposedObject() : string_("ExposedObject")
            {
            }
            std::string string_;
        };

        bool boolean_ = false;
        int integer_ = 0;
        unsigned int uinteger_ = 0U;
        float floating_ = 0.0f;
		std::string string_;
		std::wstring wstring_;
        const char* raw_string_ = nullptr;
        const wchar_t* raw_wstring_ = nullptr;
		std::shared_ptr<BinaryBlock> binary_data_;
		ExposedStruct exposedStruct_;
        ManagedObject<ExposedObject> exposedObject_;
        ObjectHandleT<ExposedObject> exposedHandle_;

#define IMPLEMENT_XETERS(Name, Type, Variable) \
	void set##Name(const Type& value)          \
	{                                          \
		Variable = value;                      \
	}                                          \
	Type get##Name##Value() const              \
	{                                          \
		return Variable;                       \
	}                                          \
	const Type& get##Name##Ref() const         \
	{                                          \
		return Variable;                       \
	}                                          \
	void get##Name##Arg(Type* value) const     \
	{                                          \
		*value = Variable;                     \
	}

		IMPLEMENT_XETERS(Boolean, bool, boolean_)
		IMPLEMENT_XETERS(Integer, int, integer_)
		IMPLEMENT_XETERS(UInteger, unsigned int, uinteger_)
		IMPLEMENT_XETERS(Float, float, floating_)
		IMPLEMENT_XETERS(String, std::string, string_)
		IMPLEMENT_XETERS(WString, std::wstring, wstring_)
		IMPLEMENT_XETERS(BinaryData, std::shared_ptr<BinaryBlock>, binary_data_)
		IMPLEMENT_XETERS(ExposedStruct, ExposedStruct, exposedStruct_)

#undef IMPLEMENT_XETERS

        void setExposedObject(const ObjectHandleT<ExposedObject>& value)
        {
            if (value != exposedHandle_)
            {
                *exposedObject_ = *value;
            }
        }
        ObjectHandleT<ExposedObject> getExposedObjectValue() const
        {
            return exposedHandle_;
        }
        const ObjectHandleT<ExposedObject>& getExposedObjectRef() const
        {
            return exposedHandle_;
        }
        void getExposedObjectArg(ObjectHandleT<ExposedObject>* value) const
        {
            *value = exposedHandle_;
        }

		// Raw string stuff is sketchy at the moment, highlighting some deficiencies in
		// the design of the reflection system.

		// void setRawString( const char * & value ) { raw_string_ = value; }
		// void getRawStringValue( const char * * value ) const { *value = raw_string_; }
		// const char * & getRawStringRef() const { return raw_string_; }

		// void setRawWString( const wchar_t * & value ) { raw_wstring_ = value; }
		// void getRawWStringValue( const wchar_t * * value ) const { *value = raw_wstring_; }
		// const wchar_t * & getRawWStringRef() const { return raw_wstring_; }
	};

	template <typename TargetType, typename HandleType>
	bool setProperty(BaseProperty* property, ObjectHandleT<HandleType>& pBase, const TargetType& value)
	{
		return property->set(pBase, ReflectionUtilities::reference(value), getDefinitionManager());
	}
};

} // end namespace wgt

#endif // TEST_FUNCTION_PROPERTY_HPP