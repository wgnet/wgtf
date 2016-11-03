#pragma once

#include "core_ui_framework/i_dialog.hpp"
#include "core_common/signal.hpp"
#include "core_reflection/reflected_object.hpp"

#include <functional>

namespace wgt
{
class DialogModel
{
public:
	DialogModel();
	virtual ~DialogModel();

	IDialog::Result result() const;
	virtual IDialog::Result closeResult() const;
	virtual void onClose(IDialog::Result result);
	virtual void onFocusIn();
	virtual void onFocusOut(bool isChildStealingFocus);
	virtual void onShow();

protected:
	void setResult(IDialog::Result result);

private:
	DECLARE_REFLECTED

	IDialog::Result result_;
};
} // end namespace wgt
