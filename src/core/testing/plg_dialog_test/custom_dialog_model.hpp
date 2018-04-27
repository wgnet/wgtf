#ifndef CUSTOM_REFLECTED_DIALOG_MODEL_HPP
#define CUSTOM_REFLECTED_DIALOG_MODEL_HPP

#include "core_data_model/dialog/reflected_dialog_model.hpp"

namespace wgt
{
/**
* Custom model for a basic dialog
*/
class CustomDialogModel : public DialogModel
{
	DECLARE_REFLECTED

public:
	const std::string& getChosenOption() const
	{
		return option_;
	}

private:
	virtual const char* getUrl() const override
	{
		return "plg_dialog_test/custom_dialog.qml";
	}

	virtual void onClose(IDialog::Result result) override
	{
		DialogModel::onClose(result);
		switch (result)
		{
		case 1:
			option_ = "A";
			break;
		case 2:
			option_ = "B";
			break;
		case 3:
			option_ = "C";
			break;
		}
	}

	std::string option_;
};

/**
* Custom model for a reflected dialog
*/
template <bool ModifyDataDirectly>
class CustomDialogReflectedModel : public ReflectedDialogModel
{
	DECLARE_REFLECTED

public:
	CustomDialogReflectedModel() : dataSaved_(false)
	{
	}

	~CustomDialogReflectedModel()
	{
	}

	bool dataSaved() const
	{
		return dataSaved_;
	}

private:
	virtual const char* getUrl() const override
	{
		return "plg_dialog_test/reflected_dialog.qml";
	}

	virtual void onClose(IDialog::Result result) override
	{
		ReflectedDialogModel::onClose(result);
		dataSaved_ = result == ReflectedDialogModel::SAVE && dataEdited();
	}

	virtual bool modifyDataDirectly() const override
	{
		return ModifyDataDirectly;
	}

	bool dataSaved_;
};

BEGIN_EXPOSE(CustomDialogModel, DialogModel, MetaNone())
END_EXPOSE()

BEGIN_EXPOSE(CustomDialogReflectedModel<true>, ReflectedDialogModel, MetaNone())
END_EXPOSE()

BEGIN_EXPOSE(CustomDialogReflectedModel<false>, ReflectedDialogModel, MetaNone())
END_EXPOSE()

} // end namespace wgt

#endif // CUSTOM_REFLECTED_DIALOG_MODEL_HPP