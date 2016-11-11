//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  i_progress_dialog.hpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef I_PROGRESS_DIALOG_HPP
#define I_PROGRESS_DIALOG_HPP

#pragma once

#include <string>
#include <memory>
#include <functional>

namespace wgt
{
class IProgressDialog
{
public:
	virtual ~IProgressDialog()
	{
	}

	virtual const std::string& labelText() const = 0;
	virtual int maximum() const = 0;
	virtual int minimum() const = 0;
	virtual int minimumDuration() const = 0;
	virtual int value() const = 0;
	virtual bool wasCanceled() const = 0;
	virtual void step() = 0;
	virtual void show() = 0;
	virtual void hide() = 0;

	// slots
	virtual void cancel() = 0;
	virtual void reset() = 0;
	virtual void setCancelButtonText(const std::string& cancelButtonText) = 0;
	virtual void setLabelText(const std::string& text) = 0;
	virtual void setMaximum(int maximum) = 0;
	virtual void setMinimum(int minimum) = 0;
	virtual void setMinimumDuration(int ms) = 0;
	virtual void setRange(int minimum, int maximum) = 0;
	virtual void setValue(int progress) = 0;
};
typedef std::unique_ptr<IProgressDialog> IProgressDialogPtr;
typedef std::function<void(IProgressDialog&)> ModelessProgressCallback;
} // end namespace wgt
#endif // I_PROGRESS_DIALOG_HPP
