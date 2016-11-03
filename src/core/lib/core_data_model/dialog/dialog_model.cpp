#include "dialog_model.hpp"

namespace wgt
{
DialogModel::DialogModel()
{
	result_ = IDialog::INVALID_RESULT;
}

DialogModel::~DialogModel()
{
}

IDialog::Result DialogModel::result() const
{
	return result_;
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
}

void DialogModel::onShow()
{
}

void DialogModel::onFocusIn()
{
}

void DialogModel::onFocusOut(bool isChildStealingFocus)
{
}

} // end namespace wgt
