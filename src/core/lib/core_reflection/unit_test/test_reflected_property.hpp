#ifndef TEST_REFLECTED_PROPERTY_HPP
#define TEST_REFLECTED_PROPERTY_HPP

#include "test_reflection_fixture.hpp"

namespace wgt
{
class TestPropertyFixture : public TestReflectionFixture
{
public:
	class TestPropertyObject
	{
	public:
		bool boolean_;
		int integer_;
		unsigned int uinteger_;
		float floating_;
		std::string string_;
		std::wstring wstring_;
		const char* raw_string_;
		const wchar_t* raw_wstring_;
		std::shared_ptr<BinaryBlock> binary_data_;

		TestPropertyObject()
		    : boolean_(false), integer_(0), uinteger_(0U), floating_(0.0f), string_(), wstring_(), raw_string_(NULL),
		      raw_wstring_(NULL), binary_data_()
		{
		}
	};

	ReflectedProperty<bool, TestPropertyObject> booleanProperty_;
	ReflectedProperty<int, TestPropertyObject> integerProperty_;
	ReflectedProperty<unsigned int, TestPropertyObject> uintegerProperty_;
	ReflectedProperty<float, TestPropertyObject> floatProperty_;

	ReflectedProperty<std::string, TestPropertyObject> stringProperty_;
	ReflectedProperty<std::wstring, TestPropertyObject> wstringProperty_;
	ReflectedProperty<const char*, TestPropertyObject> rawStringProperty_;
	ReflectedProperty<const wchar_t*, TestPropertyObject> rawWStringProperty_;

	ReflectedProperty<std::shared_ptr<BinaryBlock>, TestPropertyObject> binaryDataProperty_;

public:
	TestPropertyFixture();
};


//==============================================================================
class TestCollectionFixture
{
public:
	typedef std::vector<int> IntVector;
	typedef std::vector<float> FloatVector;
	typedef std::vector<std::string> StringVector;
	typedef std::vector<const char*> RawStringVector;
	typedef std::map<int, int> IntMap;
	typedef std::map<int, float> FloatMap;

	class TestCollectionObject
	{
	public:
		IntVector int_vector_;
		FloatVector float_vector_;
		StringVector string_vector_;
		RawStringVector raw_string_vector_;
		IntMap int_map_;
		FloatMap float_map_;

		TestCollectionObject()
		{
		}
	};

	ReflectedProperty<IntVector, TestCollectionObject> intVectorProperty_;
	ReflectedProperty<FloatVector, TestCollectionObject> floatVectorProperty_;
	ReflectedProperty<StringVector, TestCollectionObject> stringVectorProperty_;
	ReflectedProperty<RawStringVector, TestCollectionObject> rawStringVectorProperty_;
	ReflectedProperty<IntMap, TestCollectionObject> intMapProperty_;
	ReflectedProperty<FloatMap, TestCollectionObject> floatMapProperty_;

public:
	TestCollectionFixture();

	IDefinitionManager& getDefinitionManager()
	{
		return framework_.getDefinitionManager();
	}

private:
	TestFramework framework_;
};


} // end namespace wgt

#endif //TEST_REFLECTED_PROPERTY_HPP