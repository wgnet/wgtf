//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  qt_progress_dialog.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "qt_progress_dialog.hpp"
#include <QProgressDialog>
#include <QObject>
#include <QTimer>
#include <QProgressBar>

namespace wgt
{
QtProgressDialog::QtProgressDialog(const std::string& title, const std::string& label, const std::string& cancelText,
                                   uint32_t min, uint32_t max, std::chrono::milliseconds duration,
                                   ModelessProgressCallback callback)
{
	progressDialog_.reset(new QProgressDialog(QString::fromStdString(label),
	                                          cancelText.empty() ? QString() : QString::fromStdString(cancelText), min,
	                                          max, 0, 0));

	if (min == max)
	{
		indeterminate_ = true;
		progressDialog_->setAutoClose(false);
		progressDialog_->setAutoReset(false);
		progressDialog_->setWindowModality(Qt::ApplicationModal);
		progressDialog_->setWindowTitle(QString::fromStdString(title));
		progressDialog_->setMinimum(0);
		progressDialog_->setMaximum(0);
		progressDialog_->show();
	}
	else
	{
		progressDialog_->setAutoClose(false);
		progressDialog_->setAutoReset(false);
		progressDialog_->setWindowModality(Qt::ApplicationModal);
		progressDialog_->setWindowTitle(QString::fromStdString(title));
		progressDialog_->setMinimumDuration(duration.count());
		setValue(min);
		setLabelText(label);
		setCancelButtonText(cancelText);
	}

	if (callback)
	{
		progressDialog_->setWindowModality(Qt::NonModal);
		timer_.reset(new QTimer());

		// destroy the instance if it has been canceled or completed
		QObject::connect(timer_.get(), &QTimer::timeout, [callback, this]() {
			callback(*this);
			if (wasCanceled() || value() >= maximum())
			{
				timer_->stop();
				delete this;
			}
		});
		timer_->start();
	}
}

QtProgressDialog::~QtProgressDialog()
{
}

const std::string& QtProgressDialog::labelText() const
{
	return label_;
}

int QtProgressDialog::maximum() const
{
	if (progressDialog_)
	{
		return progressDialog_->maximum();
	}

	return 0;
}

int QtProgressDialog::minimum() const
{
	if (progressDialog_)
	{
		return progressDialog_->minimum();
	}

	return 0;
}

int QtProgressDialog::minimumDuration() const
{
	if (progressDialog_)
	{
		return progressDialog_->minimumDuration();
	}

	return 4000;
}

int QtProgressDialog::value() const
{
	return value_;
}

bool QtProgressDialog::wasCanceled() const
{
	if (progressDialog_)
	{
		return progressDialog_->wasCanceled();
	}

	return true;
}

void QtProgressDialog::step()
{
	if (progressDialog_)
	{
		progressDialog_->setValue(++value_);
	}
}

void QtProgressDialog::show()
{
	if (progressDialog_)
	{
		progressDialog_->show();
	}
}

void QtProgressDialog::hide()
{
	if (progressDialog_)
	{
		progressDialog_->hide();
	}
}

void QtProgressDialog::cancel()
{
	if (progressDialog_)
	{
		progressDialog_->cancel();
	}
}

void QtProgressDialog::reset()
{
	if (progressDialog_)
	{
		value_ = 0;
		return progressDialog_->reset();
	}
}

void QtProgressDialog::setCancelButtonText(const std::string& cancelButtonText)
{
	if (progressDialog_)
	{
		// Disconnect any previous connection, will reconnect if needed
		if (cancelConnection_)
		{
			progressDialog_->disconnect(cancelConnection_);
			cancelConnection_ = QMetaObject::Connection();
		}
		// If there is no cancel button reset the dialog when it is canceled to prevent setting wasCanceled
		if (cancelButtonText.empty())
		{
			cancelConnection_ =
			QObject::connect(progressDialog_.get(), &QProgressDialog::canceled, [this]() { progressDialog_->reset(); });
		}
		return progressDialog_->setCancelButtonText(
		cancelButtonText.empty() ? QString() : QString::fromStdString(cancelButtonText));
	}
}

void QtProgressDialog::setLabelText(const std::string& text)
{
	if (progressDialog_)
	{
		label_ = text;
		return progressDialog_->setLabelText(QString::fromStdString(label_));
	}
}

void QtProgressDialog::setMaximum(int maximum)
{
	if (progressDialog_)
	{
		return progressDialog_->setMaximum(maximum);
	}
}

void QtProgressDialog::setMinimum(int minimum)
{
	if (progressDialog_)
	{
		return progressDialog_->setMinimum(minimum);
	}
}

void QtProgressDialog::setMinimumDuration(int ms)
{
	if (progressDialog_)
	{
		return progressDialog_->setMinimumDuration(ms);
	}
}

void QtProgressDialog::setRange(int minimum, int maximum)
{
	if (progressDialog_)
	{
		return progressDialog_->setRange(minimum, maximum);
	}
}

void QtProgressDialog::setValue(int progress)
{
	value_ = progress;
	if (progressDialog_)
	{
		progressDialog_->setValue(value_);
	}
}
}
