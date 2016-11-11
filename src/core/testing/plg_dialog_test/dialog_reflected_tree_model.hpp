#ifndef DIALOG_REFLECTED_TREE_MODEL_HPP
#define DIALOG_REFLECTED_TREE_MODEL_HPP

#include "core_reflection/reflected_object.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_data_model/dialog/dialog_model.hpp"
#include "core_command_system/i_env_system.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"

namespace wgt
{
class AbstractTreeModel;
class IReflectionController;

class DialogReflectedTreeModel : public DialogModel
{
	DECLARE_REFLECTED

public:
	enum Result
	{
		CANCEL = 0,
		SAVE = 1
	};

	DialogReflectedTreeModel();
	~DialogReflectedTreeModel();

	void initialise(IComponentContext* context, const IClassDefinition* definition);

	template <typename T>
	void setObject(ObjectHandleT<T> data, bool modifyDirectly)
	{
		ObjectHandleT<T> copy = context_->queryInterface<IDefinitionManager>()->create<T>();
		*copy.get() = *data.get();
		setObject(data, copy, modifyDirectly);
	}

	const AbstractTreeModel* getModel() const;
	const DialogReflectedTreeModel* getSource() const;

	virtual void onShow() override;
	virtual void onClose(IDialog::Result result) override;
	virtual void onFocusIn() override;
	virtual void onFocusOut(bool isChildStealingFocus) override;

private:
	void setObject(ObjectHandle data, ObjectHandle copy, bool modifyDirectly);
	void copyReflectedData(IDefinitionManager& definitionManager, IReflectionController& reflectionController,
	                       const ObjectHandle& src, const ObjectHandle& dst, bool notify);

	ObjectHandle originalData_;
	ObjectHandle dialogData_;
	bool modifyDataDirectly_;
	bool dataEdited_;
	int environmentID_;
	int commandIndex_;
	std::shared_ptr<AbstractTreeModel> model_;
	IComponentContext* context_;
	const IClassDefinition* definition_;
};

} // end namespace wgt
#endif // DIALOG_REFLECTED_TREE_MODEL_HPP
