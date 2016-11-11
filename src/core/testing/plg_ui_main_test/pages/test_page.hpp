#ifndef TEST_PAGE_HPP
#define TEST_PAGE_HPP

#include "core_reflection/reflected_object.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "wg_types/binary_block.hpp"
#include "wg_types/vector3.hpp"
#include "wg_types/vector4.hpp"
#include <map>
#include <vector>
#include <memory>

namespace wgt
{
class TestPolyStruct;

typedef ObjectHandleT<TestPolyStruct> TestPolyStructPtr;

class TestPage
{
	DECLARE_REFLECTED
public:
	TestPage();
	~TestPage();

	void init(IDefinitionManager& defManager);

private:
	void setCheckBoxState(const bool& bChecked);
	void getCheckBoxState(bool* bChecked) const;

	void setTextField(const std::wstring& text);
	void getTextField(std::wstring* text) const;

	void setSlideData(const double& length);
	void getSlideData(double* length) const;

	static int getSlideMaxData();
	static int getSlideMinData();

	void setNumber(const int& num);
	void getNumber(int* num) const;

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

	void getThumbnail(std::shared_ptr<BinaryBlock>* path) const;

	const GenericObjectPtr& getGenericObject() const;
	void setGenericObject(const GenericObjectPtr& genericObj);

	void setTestPolyStruct(const TestPolyStructPtr& testPolyStruct);
	const TestPolyStructPtr& getTestPolyStruct() const;

	const std::vector<std::vector<float>>& getTestVector() const
	{
		return testVector_;
	}

	void getEnumFunc(int* o_EnumValue) const
	{
		*o_EnumValue = enumValue_;
	}

	void setEnumFunc(const int& o_EnumValue)
	{
		enumValue_ = o_EnumValue;
	}

	void generateEnumFunc(std::map<int, std::wstring>* o_enumMap) const;

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
	int curSelected_;
	int enumValue_;
	Vector3 vec3_;
	Vector4 vec4_;
	Vector3 color3_;
	Vector4 color4_;
	std::vector<int> intVector_;
	std::vector<float> floatVector_;
	std::vector<std::vector<int>> vectorVector_;
	std::vector<std::vector<float>> testVector_;
	std::map<int, std::string> testMap_;
	std::map<int, std::vector<std::string>> testVectorMap_;
	std::map<std::string, std::string> testStringMap_;
	TestPolyStructPtr polyStruct_;
	GenericObjectPtr genericObj_;
	std::string fileUrl_;
	std::string assetUrl_;
};

class TestPage2
{
public:
	TestPage2();
	~TestPage2();
	void init(IDefinitionManager& defManager);
	const ObjectHandleT<TestPage>& getTestPage() const;
	void setTestPage(const ObjectHandleT<TestPage>& objHandle);

private:
	ObjectHandleT<TestPage> testPage_;
};
} // end namespace wgt
#endif // TEST_PAGE_IMPL_HPP
