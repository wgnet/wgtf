#include "test_page.hpp"
#include "interfaces/i_datasource.hpp"
#include "pages/test_polymorphism.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/function_property.hpp"
#include "core_reflection/utilities/reflection_function_utilities.hpp"
#include "core_common/platform_path.hpp"
#include "core_common/platform_dll.hpp"
#include "core_variant/collection.hpp"
#include "core_logging/logging.hpp"
#include "wg_types/color_utilities.hpp"
#include "core_string_utils/string_utils.hpp"
#include <locale>
#include <codecvt>

namespace wgt
{
bool TestPage::readOnly_ = false;

TestPage::TestPage()
    : bChecked_(true)
	, boolTest_(false)
	, text_(L"Hello Test")
	, curSlideData_(0)
	, curNum_(100)
	, stringId_(0)
	, curSelected_(0)
	, enumValue_(0)
	, colorEnumValue_(0)
	, intValue_(7)
	, int64Value_(9223372036854775807)
	, uint64Value_(18446744073709551615)
	, floatValue_(1)
	, doubleValue_(1.23456)
	, vec3_(0.0f, 0.0f, 0.0f)
	, vec4_(0.0f, 0.0f, 0.0f, 0.0f)
	, color3_(192.0f, 192.0f, 192.0f)
	, color4_(127.0f, 127.0f, 127.0f, 127.0f)
	, colorHDR_(ColorUtilities::makeHDRColor(0, 59, 180, -1.0f))
	, colorKelvin_(5000)
	, polyStruct_(nullptr)
	, genericObject_(nullptr)
	, angle_(90.0f)
	, time_(10000.0f)
{
	for (auto i = 0; i < 10; ++i)
	{
		intVector_.push_back(rand());
	}

	for (auto i = 0; i < 10; ++i)
	{
		std::vector<float> bla;
		for (auto i = 0; i < 10; ++i)
		{
			bla.push_back((float)rand() / RAND_MAX);
		}
		testVector_.push_back(bla);
	}

	for (auto i = 0; i < 10; ++i)
	{
		floatVector_.push_back((float)rand() / RAND_MAX);
	}
	for (auto i = 0; i < 10; ++i)
	{
		std::vector<int> bla;
		for (auto j = 0; j < 10; ++j)
		{
			bla.push_back(rand());
		}
		vectorVector_.push_back(bla);
	}

	std::string testString;
	for (auto i = 0; i < 10; ++i)
	{
		char testBuffer[1024];
		char* pos = &testBuffer[0];
		for (auto j = 0; j < 10; ++j)
		{
			pos += sprintf(pos, "%d%c", rand(), (char)((float)rand() / RAND_MAX * 26 + 'a'));
		}
		testString = testBuffer;
		testMap_.insert(std::make_pair(rand(), testString));
	}

	for (auto i = 0; i < 10; ++i)
	{
		std::vector<std::string> stringVector;
		for (auto j = 0; j < 10; ++j)
		{
			char testBuffer[1024];
			char* pos = &testBuffer[0];
			for (auto k = 0; k < 10; ++k)
			{
				pos += sprintf(pos, "%d%c", rand(), (char)((float)rand() / RAND_MAX * 26 + 'a'));
			}
			stringVector.push_back(testBuffer);
		}
		testVectorMap_.insert(std::make_pair(rand(), stringVector));
	}

	testStringMap_["key a"] = "value a";
	testStringMap_["key b"] = "value b";
	testStringMap_["key c"] = "value c";

	testBoolMap_["True"] = true;
	testBoolMap_["False"] = false;
	testBoolMap_["Partial"] = Variant();

	wchar_t path[MAX_PATH];
	::GetModuleFileNameW(NULL, path, MAX_PATH);
	::PathRemoveFileSpecW(path);
	::PathAppendW(path, L"plugins\\");

	// std::wstring_convert is reported as memory leak - it does custom dtor call and free for codecvt object
	std::unique_ptr<char[]> str(new char[2 * MAX_PATH]);
	wcstombs(str.get(), path, 2 * MAX_PATH);
	fileUrl_ = str.get();

	fileUrl_ += "plugins_ui.txt";
	assetUrl_ = "file:///sample.png";
	
	genericObject_ = GenericObject::create();
    genericObject_->set("String", std::string("Wargaming"));
    genericObject_->set("Integer", 100);

	const int objectCount = 4;
	for(int i = 0; i < objectCount; ++i)
	{
		objectVectorManaged_.push_back(ManagedObject<TestObject>::make(objectCount, i));
		objectVector_.push_back(objectVectorManaged_.back().getHandle());
	}
}

TestPage::~TestPage()
{
	polyStruct_ = nullptr;
    genericObject_ = nullptr;
}

std::string TestPage::objectDisplayName(std::string path, const ObjectHandle& handle)
{
	if(auto obj = handle.getBase<TestObject::Member>())
	{
		// Path will be [i].Enabled
		return path.substr(path.find(".") + 1);
	}
	else if(auto obj = handle.getBase<TestObject>())
	{
		// Path will be [i].Member[i]
		const auto first = path.rfind(Collection::getIndexOpen());
		const auto last = path.rfind(Collection::getIndexClose());
		const int index = atoi(path.substr(first + 1, last).c_str());
		return obj->membersManaged_.at(index)->name_;
	}
	else if (auto obj = handle.getBase<TestPage>())
	{
		// Path will be [i]
		if(StringUtils::erase_string(path, Collection::getIndexOpen()) &&
		   StringUtils::erase_string(path, Collection::getIndexClose()))
		{
			const int index = atoi(path.c_str());
			return obj->objectVectorManaged_.at(index)->name_;
		}
	}
	return path;
}

void TestPage::setCheckBoxState(const bool& bChecked)
{
	bChecked_ = bChecked;
}
void TestPage::getCheckBoxState(bool* bChecked) const
{
	*bChecked = bChecked_;
}

void TestPage::setTextField(const std::wstring& text)
{
	text_ = text;
}
void TestPage::getTextField(std::wstring* text) const
{
	*text = text_;
}

void TestPage::setSlideData(const double& length)
{
	curSlideData_ = length;
}
void TestPage::getSlideData(double* length) const
{
	*length = curSlideData_;
}

void TestPage::setNumber(const int& num)
{
	curNum_ = num;
}
void TestPage::getNumber(int* num) const
{
	*num = curNum_;
}

void TestPage::setStringId(const uint64_t& stringId)
{
	stringId_ = stringId;
}
void TestPage::getStringId(uint64_t* stringId) const
{
	*stringId = stringId_;
}

void TestPage::setAngle(const float& angle)
{
	angle_ = angle;
}
void TestPage::getAngle(float* angle) const
{
	*angle = angle_;
}

void TestPage::setTime(const float& time)
{
	time_ = time;
}
void TestPage::getTime(float* time) const
{
	*time = time_;
}

void TestPage::setSelected(const int& select)
{
	curSelected_ = select;
}
void TestPage::getSelected(int* select) const
{
	*select = curSelected_;
}

void TestPage::setVector3(const Vector3& vec3)
{
	vec3_.x = vec3.x;
	vec3_.y = vec3.y;
	vec3_.z = vec3.z;
}
void TestPage::getVector3(Vector3* vec3) const
{
	vec3->x = vec3_.x;
	vec3->y = vec3_.y;
	vec3->z = vec3_.z;
}

void TestPage::setVector4(const Vector4& vec4)
{
	vec4_.x = vec4.x;
	vec4_.y = vec4.y;
	vec4_.z = vec4.z;
	vec4_.w = vec4.w;
}
void TestPage::getVector4(Vector4* vec4) const
{
	vec4->x = vec4_.x;
	vec4->y = vec4_.y;
	vec4->z = vec4_.z;
	vec4->w = vec4_.w;
}

void TestPage::setColor3(const Vector3& color)
{
	color3_.x = color.x;
	color3_.y = color.y;
	color3_.z = color.z;
}
void TestPage::getColor3(Vector3* color) const
{
	color->x = color3_.x;
	color->y = color3_.y;
	color->z = color3_.z;
}

void TestPage::setColor4(const Vector4& color)
{
	color4_.x = color.x;
	color4_.y = color.y;
	color4_.z = color.z;
	color4_.w = color.w;
}
void TestPage::getColor4(Vector4* color) const
{
	color->x = color4_.x;
	color->y = color4_.y;
	color->z = color4_.z;
	color->w = color4_.w;
}

void TestPage::setHDRColor(const Vector4& color)
{
	colorHDR_.x = color.x;
	colorHDR_.y = color.y;
	colorHDR_.z = color.z;
	colorHDR_.w = color.w;
}
void TestPage::getHDRColor(Vector4* color) const
{
	color->x = colorHDR_.x;
	color->y = colorHDR_.y;
	color->z = colorHDR_.z;
	color->w = colorHDR_.w;
}

void TestPage::setKelvinColor(const unsigned int& color)
{
	colorKelvin_ = color;
}
void TestPage::getKelvinColor(unsigned int* color) const
{
	*color = colorKelvin_;
}

void TestPage::getThumbnail(std::shared_ptr<BinaryBlock>* thumbnail) const
{
	auto dataSrcMngr = get<IDataSourceManager>();
	assert(dataSrcMngr);
	*thumbnail = dataSrcMngr->getThumbnailImage();
}

const GenericObjectPtr& TestPage::getGenericObject() const
{
	return genericObject_->handle();
}
void TestPage::setGenericObject(const GenericObjectPtr& genericObj)
{
	*genericObject_ = genericObj;
}

void TestPage::setTestPolyStruct(const TestPolyStructPtr& testPolyStruct)
{
	polyStruct_ = testPolyStruct;
}
const TestPolyStructPtr& TestPage::getTestPolyStruct() const
{
    return polyStruct_;
}

void TestPage::generateEnumLargeFunc(std::map<int, Variant>* o_enumMap) const
{
	o_enumMap->clear();
	for (int i = 0; i < 50; ++i)
	{
		o_enumMap->insert(std::make_pair(i, ("Value " + std::to_string(i)).c_str()));
	}
}

void TestPage::generateEnumFunc(std::map<int, Variant>* o_enumMap) const
{
	// change the case just for the purpose of demonstrating dynamically generating dropdown list
	// when users click on the dropdownbox
	static int i = 0;
	if (i == 0)
	{
		o_enumMap->clear();
		o_enumMap->insert(std::make_pair(0, L"First Value"));
		o_enumMap->insert(std::make_pair(1, L"Second Value"));
		o_enumMap->insert(std::make_pair(2, L"third Value"));
		o_enumMap->insert(std::make_pair(3, L"Forth Value"));
		i = 1;
		return;
	}
	o_enumMap->clear();
	o_enumMap->insert(std::make_pair(0, L"1st Value"));
	o_enumMap->insert(std::make_pair(1, L"2nd Value"));
	o_enumMap->insert(std::make_pair(2, L"3rd Value"));
	o_enumMap->insert(std::make_pair(3, L"4th Value"));
	i = 0;
}

void TestPage::generateColorEnumFunc(std::map<int, Variant>* o_enumMap) const
{
	o_enumMap->clear();
	o_enumMap->insert(std::make_pair(0, Vector3(255, 0, 0)));
	o_enumMap->insert(std::make_pair(1, Vector3(0, 255, 0)));
	o_enumMap->insert(std::make_pair(2, Vector3(0, 0, 255)));
	o_enumMap->insert(std::make_pair(3, Vector3(255, 255, 255)));
	return;
}

const std::string& TestPage::getFileUrl() const
{
	return fileUrl_;
}

void TestPage::setFileUrl(const std::string& url)
{
	fileUrl_ = url;
}

const std::string& TestPage::getAssetUrl() const
{
	return assetUrl_;
}

void TestPage::setAssetUrl(const std::string& url)
{
	assetUrl_ = url;
}

void TestPage::methodOnly()
{
	// Do nothing
}

bool TestPage::getReadOnly(const ObjectHandle&)
{
	return readOnly_;
}

void TestPage::toggleReadOnly()
{
	readOnly_ = !readOnly_;
}

const std::vector<std::vector<float>>& TestPage::getTestVector() const
{
	return testVector_;
}

void TestPage::getEnumFunc(int* o_EnumValue) const
{
	*o_EnumValue = enumValue_;
}

void TestPage::setEnumFunc(const int& o_EnumValue)
{
	enumValue_ = o_EnumValue;
}

void TestPage::getColorEnumFunc(int* o_EnumValue) const
{
	*o_EnumValue = colorEnumValue_;
}

void TestPage::setColorEnumFunc(const int& o_EnumValue)
{
	colorEnumValue_ = o_EnumValue;
}

} // end namespace wgt
