#pragma once
#ifndef _HELLO_PANEL_EXPOSED_HPP
#define _HELLO_PANEL_EXPOSED_HPP

#include "core_reflection/reflection_macros.hpp"

namespace wgt
{
class HelloPanelExposed
{
	DECLARE_REFLECTED

public:
	HelloPanelExposed() : clickCount_(0)
	{
		this->updateClickDescription("");
	}

	void clickButton(int value)
	{
		clickCount_ += value;
		this->updateClickDescription("Click");
	}

	void undoClickButton(const ObjectHandle& params, Variant result)
	{
		clickCount_ -= (*params.getBase<ReflectedMethodParameters>())[0].cast<int>();
		this->updateClickDescription("Undo");
	}

	void redoClickButton(const ObjectHandle& params, Variant result)
	{
		clickCount_ += (*params.getBase<ReflectedMethodParameters>())[0].cast<int>();
		this->updateClickDescription("Redo");
	}

	void updateClickDescription(std::string methodLastCalled)
	{
		clickDescription_ = "\nHello from C++:\n";
		if (!methodLastCalled.empty())
		{
			clickDescription_ += methodLastCalled + " was called";
		}
	}

private:
	int clickCount_;
	std::string clickDescription_;
};

class HelloPanelExposedSource
{
	DECLARE_REFLECTED

public:
	HelloPanelExposedSource() : sourceModel_(new HelloPanelExposed())
	{
	}

	HelloPanelExposed* sourceModel() const
	{
		return sourceModel_.get();
	}

private:
	std::unique_ptr<HelloPanelExposed> sourceModel_;
};

} // end namespace wgt

#endif // _HELLO_PANEL_EXPOSED_HPP