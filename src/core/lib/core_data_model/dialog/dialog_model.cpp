#include "dialog_model.hpp"

namespace wgt
{
DialogModel::DialogModel(): title_(""), result_(IDialog::INVALID_RESULT), useStoredTitle_(false)
{
}

DialogModel::~DialogModel()
{
}

IDialog::Result DialogModel::result() const
{
	return result_;
}

bool DialogModel::canClose() const
{
	return true;
}

IDialog::Result DialogModel::closeResult() const
{
	return 0;
}

void DialogModel::setResult(IDialog::Result result)
{
	result_ = result;
}

void DialogModel::onClose(IDialog::Result result)
{
	setResult(result);
}

void DialogModel::onShow()
{
}

const char* DialogModel::getUrl() const
{
	return url_.c_str();
}

void DialogModel::setURL(const char* url)
{
	url_ = url ? url : "";
}

const char* DialogModel::getTitle() const
{
	if (useStoredTitle_)
	{
		return title_.c_str();
	}
	return nullptr;
}

void DialogModel::setTitle(const char* title)
{
	if (title == nullptr)
	{
		return;
	}
	useStoredTitle_ = true;
	title_ = title;
}

} // end namespace wgt
