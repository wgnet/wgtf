#ifndef REFLECTED_DIALOG_MODEL_HPP
#define REFLECTED_DIALOG_MODEL_HPP

#include "core_data_model/dialog/dialog_model.hpp"
#include "core_environment_system/i_env_system.hpp"
#include "core_object/managed_object.hpp"

namespace wgt
{
class AbstractTreeModel;
class IDefinitionManager;
class ICommandManager;
class IEnvManager;

class ReflectedDialogModel : public DialogModel, Depends<IDefinitionManager, ICommandManager>
{
	DECLARE_REFLECTED

public:
	enum Result
	{
		CANCEL = 0,
		SAVE = 1
	};

	ReflectedDialogModel();
	virtual ~ReflectedDialogModel();

	template <typename Model, typename Data>
	static typename std::enable_if<std::is_same<ReflectedDialogModel, Model>::value, ManagedObjectPtr>::type
	create(ObjectHandleT<Data> data, const char* resource = nullptr, const char* title = nullptr)
	{
		return createModel<Model, Data>(data, resource, title);
	}

	template <typename Model, typename Data>
	static typename std::enable_if<std::is_base_of<ReflectedDialogModel, Model>::value 
		&& !std::is_same<ReflectedDialogModel, Model>::value , ManagedObjectPtr>::type
	create(ObjectHandleT<Data> data, const char* resource = nullptr, const char* title = nullptr)
	{
		registerDefinition<Model>();
        return createModel<Model, Data>(data, resource, title);
	}

protected:
	bool dataEdited() const;
	void setModel(std::shared_ptr<AbstractTreeModel> model);
	const AbstractTreeModel* getModel() const;
	virtual void onShow() override;
	virtual void onClose(IDialog::Result result) override;

	virtual void initialise(ManagedObjectPtr dialogData, 
							ObjectHandle originalData, 
							const char* resource = nullptr, 
							const char* title = nullptr);

private:
	ReflectedDialogModel(const ReflectedDialogModel& rhs) = delete;
	ReflectedDialogModel& operator=(const ReflectedDialogModel& rhs) = delete;

	template <typename Model, typename Data>
	static ManagedObjectPtr createModel(ObjectHandleT<Data> data, const char* resource, const char* title)
	{
		return ManagedObject<Model>::make_iunique_fn([&data, title, resource](ReflectedDialogModel& model)
		{
			model.initialise(model.modifyDataDirectly()
				? nullptr : ManagedObject<Data>::make_unique(), data, resource, title);
		});
	}

	virtual bool modifyDataDirectly() const;
	void getDataEditSignal(Signal<void(Variant&)>** result) const;

	ManagedObjectPtr dialogDataObj_;
	ObjectHandle dialogData_;
	ObjectHandle originalData_;
	bool dataEdited_ = false;
	int commandIndex_ = -1;
	std::shared_ptr<AbstractTreeModel> model_;
	Signal<void(Variant&)> dataEditSignal_;
};

} // end namespace wgt
#endif // REFLECTED_DIALOG_MODEL_HPP
