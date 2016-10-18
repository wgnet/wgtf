#include "core_reflection/reflected_object.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_reflection/object_handle.hpp"
#include "dialog_reflected_data.hpp"

namespace wgt
{
class AbstractTreeModel;
class IDialog;

class DialogTestModel
{
	DECLARE_REFLECTED

public:
	DialogTestModel();
	~DialogTestModel();

	void initialise(IComponentContext& context, const IClassDefinition* definition);
	void finalise();
	void callBasicDialog(bool modal);
	void callReflectedDialog(bool modal, bool modifyDataDirectly);
	const AbstractTreeModel* getReflectedModel() const;
	const DialogTestModel* getSource() const;

private:
	ObjectHandleT<DialogReflectedData> reflectedData_;
	std::shared_ptr<AbstractTreeModel> reflectedModel_;
	std::shared_ptr<IDialog> reflectedDialog_;

	std::string basicDialogResult_;
	IComponentContext* context_;
	const IClassDefinition* definition_;
};
} // end namespace wgt
