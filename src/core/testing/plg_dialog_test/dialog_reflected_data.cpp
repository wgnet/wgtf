#include "dialog_reflected_data.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include <algorithm>
#include <vector>

namespace wgt
{
struct DialogReflectedData::Data
{
	Data(IComponentContext& context)
	    : checked_(true), text_(L"Hello Test"), slider_(0.0), vector_(nullptr), color_(51.0f, 153.0f, 255.0f),
	      context_(context)
	{
		auto& definitionManager = *context_.queryInterface<IDefinitionManager>();
		vector_ = GenericObject::create(definitionManager);
		for (int i = 0; i < 4; ++i)
		{
			vector_->set(std::to_string(i).c_str(), rand());
		}
	}

	Data(Data& data)
	    : checked_(data.checked_), text_(data.text_), slider_(data.slider_), vector_(nullptr), color_(data.color_),
	      context_(data.context_)
	{
		auto& definitionManager = *context_.queryInterface<IDefinitionManager>();
		vector_ = GenericObject::create(definitionManager);
		auto definition = data.vector_.getDefinition(definitionManager);
		for (auto property : definition->allProperties())
		{
			const char* name = property->getName();
			vector_->set(name, definition->bindProperty(name, data.vector_).getValue());
		}
	}

	bool checked_;
	std::wstring text_;
	double slider_;
	GenericObjectPtr vector_;
	Vector3 color_;
	IComponentContext& context_;
};

DialogReflectedData::DialogReflectedData()
{
}

DialogReflectedData::~DialogReflectedData()
{
}

const DialogReflectedData& DialogReflectedData::operator=(const DialogReflectedData& data)
{
	if (this != &data)
	{
		assert(data.data_);
		data_.reset(new Data(*data.data_));
	}
	return *this;
}

void DialogReflectedData::initialise(IComponentContext& context)
{
	data_.reset(new Data(context));
}

void DialogReflectedData::setColor(const Vector3& color)
{
	data_->color_.x = color.x;
	data_->color_.y = color.y;
	data_->color_.z = color.z;
}

void DialogReflectedData::getColor(Vector3* color) const
{
	color->x = data_->color_.x;
	color->y = data_->color_.y;
	color->z = data_->color_.z;
}

void DialogReflectedData::setCheckBoxState(const bool& checked)
{
	data_->checked_ = checked;
}

void DialogReflectedData::getCheckBoxState(bool* checked) const
{
	*checked = data_->checked_;
}

void DialogReflectedData::setTextField(const std::wstring& text)
{
	data_->text_ = text;
}

void DialogReflectedData::getTextField(std::wstring* text) const
{
	*text = data_->text_;
}

void DialogReflectedData::setSlideData(const double& length)
{
	if (length >= getSlideMinData() && length <= getSlideMaxData())
	{
		data_->slider_ = length;
	}
}

void DialogReflectedData::getSlideData(double* length) const
{
	*length = data_->slider_;
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
	data_->vector_ = vec;
}

const GenericObjectPtr& DialogReflectedData::getVector() const
{
	return data_->vector_;
}

} // end namespace wgt
