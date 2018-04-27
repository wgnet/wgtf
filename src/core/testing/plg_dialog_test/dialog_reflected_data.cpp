#include "dialog_reflected_data.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_object/managed_object.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/base_property.hpp"
#include "wg_types/color_utilities.hpp"
#include "core_string_utils/string_utils.hpp"
#include <algorithm>
#include <vector>

namespace wgt
{
DialogReflectedData::DialogReflectedData()
	: text_(L"Hello Test")
	, slider_(0.0)
	, vector_(nullptr)
	, color_(51.0f, 153.0f, 255.0f, 255.0f)
{
	vector_ = GenericObject::create();
	for (int i = 0; i < 4; ++i)
	{
		vector_->set(std::to_string(i).c_str(), rand());
	}

	checkBoxes_["True"] = true;
	checkBoxes_["False"] = false;
	checkBoxes_["Partial"] = Variant();
}

DialogReflectedData::~DialogReflectedData()
{
}

std::string DialogReflectedData::getDisplayName(std::string path, const ObjectHandle&)
{
	StringUtils::erase_string(path, Collection::getIndexCloseStr());
	StringUtils::erase_string(path, Collection::getIndexOpen());
	return path;
}

void DialogReflectedData::setColor(const Vector4& color)
{
	color_.x = color.x;
	color_.y = color.y;
	color_.z = color.z;
	color_.w = color.w;
}

void DialogReflectedData::getColor(Vector4* color) const
{
	color->x = color_.x;
	color->y = color_.y;
	color->z = color_.z;
	color->w = color_.w;
}

void DialogReflectedData::setTextField(const std::wstring& text)
{
	text_ = text;
}

void DialogReflectedData::getTextField(std::wstring* text) const
{
	*text = text_;
}

void DialogReflectedData::setSlideData(const double& length)
{
	if (length >= getSlideMinData() && length <= getSlideMaxData())
	{
		slider_ = length;
	}
}

void DialogReflectedData::getSlideData(double* length) const
{
	*length = slider_;
}

int DialogReflectedData::getSlideMaxData()
{
	return 100;
}

int DialogReflectedData::getSlideMinData()
{
	return -100;
}

void DialogReflectedData::setVector(const GenericObjectPtr& vec)
{
	*vector_ = vec;
}

const GenericObjectPtr& DialogReflectedData::getVector() const
{
	return vector_->handle();
}

} // end namespace wgt
