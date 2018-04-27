#ifndef DIALOG_TEST_PANEL_HPP
#define DIALOG_TEST_PANEL_HPP

#include "core_reflection/reflected_object.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_reflection/object_handle.hpp"
#include "dialog_reflected_data.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_object/managed_object.hpp"
#include "core_object/object_handle_provider.hpp"

namespace wgt
{
class AbstractTreeModel;
class IUIFramework;
class IDefinitionManager;

class DialogTestPanel : Depends<IUIFramework, IDefinitionManager>
                      , public ObjectHandleProvider<DialogTestPanel>
{
	DECLARE_REFLECTED

public:
	~DialogTestPanel();
	void initialise();
	void callBasicDialog(bool modal);
	void callCustomDialog(bool modal);
	void callReflectedDialogModifyDirect(bool modal);
	void callReflectedDialogModifyCopy(bool modal);
	const AbstractTreeModel* getReflectedModel() const;

private:
	const IClassDefinition* definition_ = nullptr;
	ManagedObject<DialogReflectedData> reflectedData_ = nullptr;
	std::shared_ptr<AbstractTreeModel> reflectedModel_;

	std::string basicDialogResult_;
	std::string customDialogResult_;
};
} // end namespace wgt

#endif // DIALOG_TEST_PANEL_HPP