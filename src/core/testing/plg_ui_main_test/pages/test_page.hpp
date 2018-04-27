#ifndef TEST_PAGE_HPP
#define TEST_PAGE_HPP

#include "core_reflection/reflected_object.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_object/managed_object.hpp"
#include "wg_types/binary_block.hpp"
#include "wg_types/vector3.hpp"
#include "wg_types/vector4.hpp"
#include <map>
#include <vector>
#include <memory>

namespace wgt
{
class IDataSourceManager;
typedef ObjectHandleT<class TestPolyStruct> TestPolyStructPtr;

class TestPage : Depends<IDataSourceManager, IDefinitionManager>
{
	DECLARE_REFLECTED
public:
	TestPage();
	~TestPage();

	static void toggleReadOnly();
	static bool getReadOnly(const ObjectHandle&);

	static std::string objectDisplayName(std::string path, const ObjectHandle& handle);

private:
	TestPage(const TestPage&) = delete;
	TestPage& operator=(const TestPage&) = delete;

	void setCheckBoxState(const bool& bChecked);
	void getCheckBoxState(bool* bChecked) const;

	void setTextField(const std::wstring& text);
	void getTextField(std::wstring* text) const;

	void setSlideData(const double& length);
	void getSlideData(double* length) const;

	void setNumber(const int& num);
	void getNumber(int* num) const;

	void setStringId(const uint64_t& stringId);
	void getStringId(uint64_t* stringId) const;

	void setAngle(const float& angle);
	void getAngle(float* angle) const;

	void setTime(const float& time);
	void getTime(float* time) const;

	void setSelected(const int& select);
	void getSelected(int* select) const;

	void setVector3(const Vector3& vec3);
	void getVector3(Vector3* vec3) const;

	void setVector4(const Vector4& vec4);
	void getVector4(Vector4* vec4) const;

	void setColor3(const Vector3& color);
	void getColor3(Vector3* color) const;

	void setColor4(const Vector4& color);
	void getColor4(Vector4* color) const;

	void setHDRColor(const Vector4& color);
	void getHDRColor(Vector4* color) const;

	void setKelvinColor(const unsigned int& color);
	void getKelvinColor(unsigned int* color) const;

	void getThumbnail(std::shared_ptr<BinaryBlock>* path) const;

	const GenericObjectPtr& getGenericObject() const;
	void setGenericObject(const GenericObjectPtr& genericObj);

	void setTestPolyStruct(const TestPolyStructPtr& testPolyStruct);
	const TestPolyStructPtr& getTestPolyStruct() const;

	const std::vector<std::vector<float>>& getTestVector() const;

	void getEnumFunc(int* o_EnumValue) const;
	void setEnumFunc(const int& o_EnumValue);
	void generateEnumFunc(std::map<int, Variant>* o_enumMap) const;
	void generateEnumLargeFunc(std::map<int, Variant>* o_enumMap) const;

	void getColorEnumFunc(int* o_EnumValue) const;
	void setColorEnumFunc(const int& o_EnumValue);
	void generateColorEnumFunc(std::map<int, Variant>* o_enumMap) const;

	const std::string& getFileUrl() const;
	void setFileUrl(const std::string& url);

	const std::string& getAssetUrl() const;
	void setAssetUrl(const std::string& url);

	void methodOnly();

	bool bChecked_;
	bool boolTest_;
	std::wstring text_;
	double curSlideData_;
	int curNum_;
	uint64_t stringId_;
	int curSelected_;
	int enumValue_;
	int colorEnumValue_;
	int32_t intValue_;
	int64_t int64Value_;
	uint64_t uint64Value_;
	float floatValue_;
	double doubleValue_;
	Vector3 vec3_;
	Vector4 vec4_;
	Vector3 color3_;
	Vector4 color4_;
	Vector4 colorHDR_;
	unsigned int colorKelvin_;
	std::vector<int> intVector_;
	std::vector<float> floatVector_;
	std::vector<ObjectHandle> objectVector_;
	std::vector<ManagedObject<struct TestObject>> objectVectorManaged_;
	std::vector<std::vector<int>> vectorVector_;
	std::vector<std::vector<float>> testVector_;
	std::map<int, std::string> testMap_;
	std::map<int, std::vector<std::string>> testVectorMap_;
	std::map<std::string, std::string> testStringMap_;
	std::map<std::string, Variant> testBoolMap_;
    ObjectHandleT<TestPolyStruct> polyStruct_;
	ManagedObject<GenericObject> genericObject_;
	std::string fileUrl_;
	std::string assetUrl_;
	float time_;
	float angle_;
	static bool readOnly_;
};

struct TestObject
{
	struct Member
	{
		Member(int index)
			: name_("Member: " + std::to_string(index))
		{
		}

		bool enabled_ = false;
		const std::string name_;
	};

	TestObject(int count, int index)
		: name_("TestObject: " + std::to_string(index))
	{
		for (int i = 0; i < count; ++i)
		{
			membersManaged_.push_back(ManagedObject<Member>::make(i));
			members_.push_back(membersManaged_.back().getHandle());
		}
	}

	const std::string name_;
	std::vector<ObjectHandle> members_;
	std::vector<ManagedObject<Member>> membersManaged_;
};

} // end namespace wgt
#endif // TEST_PAGE_IMPL_HPP
