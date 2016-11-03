//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  qt_progress_dialog.hpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef QT_PROGRESS_DIALOG_HPP
#define QT_PROGRESS_DIALOG_HPP

#pragma once

#include "core_ui_framework/i_progress_dialog.hpp"

#include <functional>
#include <memory>
#include <chrono>
#include <stdint.h>

#include "qobjectdefs.h"

class QTimer;
class QProgressDialog;
class QProgressBar;

namespace wgt
{
	class IComponentContext;

	class QtProgressDialog : public IProgressDialog
	{
	public:

		virtual ~QtProgressDialog();

		virtual const std::string& labelText() const override;
		virtual int maximum() const override;
		virtual int minimum() const override;
		virtual int minimumDuration() const override;
		virtual int value() const override;
		virtual bool wasCanceled() const override;
		virtual void step() override;
	    virtual void show() override;
	    virtual void hide() override;

	    // slots
		virtual void cancel() override;
		virtual void reset() override;
		virtual void setCancelButtonText(const std::string& cancelButtonText) override;
		virtual void setLabelText(const std::string& text) override;
		virtual void setMaximum(int maximum) override;
		virtual void setMinimum(int minimum) override;
		virtual void setMinimumDuration(int ms) override;
		virtual void setRange(int minimum, int maximum) override;
		virtual void setValue(int progress) override;

	private:
		// Only allow the QtFramework to create QtProgressDialogs
		friend class QtFramework;
	    QtProgressDialog(
	    const std::string& title,
	    const std::string& label,
	    const std::string& cancelText,
	    uint32_t minimum,
	    uint32_t maximum,
	    std::chrono::milliseconds duration,
	    ModelessProgressCallback modelessCallback,
	    bool indeterminate = false);

	    int value_;
	    std::string label_;
	    std::unique_ptr<QProgressDialog> progressDialog_;
	    std::unique_ptr<QProgressBar> progressBar_;
	    std::unique_ptr<QTimer> timer_;
	    QMetaObject::Connection cancelConnection_;
    };
} // end namespace wgt
#endif//QT_PROGRESS_DIALOG_HPP
