#ifndef DEMO_OBJECTS_HPP
#define DEMO_OBJECTS_HPP

#include "core_reflection/object_handle.hpp"
#include "core_data_model/generic_list.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "tinyxml2.hpp"
#include "object_selection_helper.hpp"
#include "core_command_system/i_env_system.hpp"
#include "core_data_model/value_change_notifier.hpp"

namespace wgt
{
class IComponentContext;
class IDefinitionManager;
class IReflectionController;
class IListModel;
class ITreeModel;
class Vector3;
class IFileSystem;

class DemoObjectsEnvCom;

class DemoObjects : public IEnvEventListener
{
public:
	DemoObjects();
	~DemoObjects();
	bool init(IComponentContext& contextManager);
	bool fini();

	const ITreeModel* getTreeModel() const;
	const IListModel* getListModel() const;
	void updateRootObject(int index);
	const int rootObjectIndex() const;
	void setTexture(int index, std::string currfilePath, std::string newFilePath);
	void undoSetTexture(const ObjectHandle& params, Variant result);
	void redoSetTexture(const ObjectHandle& params, Variant result);

	/**
	* @return the path of the current texture attached to the object
	* @note will return an empty string if the object does not support textures
	*/
	std::string getObjectTexture(int index);

	const IValueChangeNotifier* currentIndexSource() const;
	const IValueChangeNotifier* currentListSource() const;

	ObjectHandle createObject(Vector3 pos);
	void undoCreateObject(const ObjectHandle& params, Variant result);
	void redoCreateObject(const ObjectHandle& params, Variant result);

	// IEnvEventListener
	virtual void onAddEnv(IEnvState* state) override;
	virtual void onRemoveEnv(IEnvState* state) override;
	virtual void onSelectEnv(IEnvState* state) override;

	bool loadDemoData(const char* name, DemoObjectsEnvCom* objects);

private:
	void populateDemoObject(GenericObjectPtr& genericObject, const tinyxml2::XMLNode& objectNode);

	IDefinitionManager* pDefManager_;
	IReflectionController* controller_;
	IEnvManager* envManager_;
	IFileSystem* fileSystem_;

	ObjectSelectionHelper helper_;
	std::unique_ptr<ValueChangeNotifier<IListModel*>> pEnvChangeHelper_;
	ObjectHandle nullSelection_;

	DemoObjectsEnvCom* objects_;
	mutable std::shared_ptr<ITreeModel> treeModel_;
};
} // end namespace wgt
#endif // DEMO_OBJECTS_HPP
