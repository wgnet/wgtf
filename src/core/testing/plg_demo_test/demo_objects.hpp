#ifndef DEMO_OBJECTS_HPP
#define DEMO_OBJECTS_HPP

#include "core_dependency_system/depends.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "core_reflection/reflected_object.hpp"
#include "tinyxml2.hpp"
#include "core_environment_system/i_env_system.hpp"
#include "core_data_model/value_change_notifier.hpp"
#include "core_data_model/abstract_item_model.hpp"
#include "core_data_model/collection_model.hpp"
#include "core_object/object_handle_provider.hpp"
#include "wg_types/vector3.hpp"

namespace wgt
{
class IDefinitionManager;
class IFileSystem;
class ICommandManager;

class DemoScene : Depends<IFileSystem, IDefinitionManager, ICommandManager>
                , public ObjectHandleProvider<DemoScene>
{
public:
	DECLARE_REFLECTED

	DemoScene(std::function<void(int)> onObjectSelect);
	virtual ~DemoScene();
	void initialise(const std::string& id);
	ObjectHandle createObject(Vector3 pos);
	void undoCreateObject(Variant params, Variant result);
	void redoCreateObject(Variant params, Variant result);
	void flagDirty();
	int selected() const;
	bool isSelectedValid() const;
	void selectObject(int index);
	const AbstractTreeModel* getTreeModel() const;
	const AbstractListModel* getListModel() const;

private:
	void setTexture(int index, std::string currfilePath, std::string newFilePath);
	void undoSetTexture(Variant params, Variant result);
	void redoSetTexture(Variant params, Variant result);
	void loadData();
	void populateDemoObject(GenericObjectPtr& genericObject, const tinyxml2::XMLNode& objectNode);
	void createTreeModel();
    void getSceneDirtySignal(Signal<void(Variant&)>** result) const;
    bool getSceneDirty() const;

	/**
	* @return the path of the current texture attached to the object
	* @note will return an empty string if the object does not support textures
	*/
	std::string getObjectTexture(int index);

	std::string id_;
	int selectedIndex_ = -1;
	bool sceneDirty_ = false;
    Signal<void(Variant&)> sceneDirtyChanged_;
	std::function<void(int)> onObjectSelect_ = nullptr;
	std::vector<ManagedObject<GenericObject>> objects_;
	std::vector<GenericObjectPtr> objectHandles_;
	std::shared_ptr<CollectionModel> objectListModel_;
	std::shared_ptr<AbstractTreeModel> treeModel_;
	const IClassDefinition* definition_ = nullptr;
	ObjectHandle nullSelection_;
};

class DemoObjects : public EnvComponentT<IEnvComponentState>
                  , Depends<IDefinitionManager, IFileSystem>
                  , public ObjectHandleProvider<DemoObjects>
{
	DECLARE_REFLECTED

public:
	DemoObjects(IEnvManager& envManager);
	~DemoObjects();
	bool init(IComponentContext& contextManager);
	bool fini();
	ObjectHandleT<DemoScene> createScene(const std::string& id);

private:
	const AbstractTreeModel* getTreeModel() const;
	void getTreeModelSignal(Signal<void(Variant&)>** result) const;
	void updateTreeModel(const AbstractTreeModel* model);

	const AbstractListModel* getListModel() const;
	void getListModelSignal(Signal<void(Variant&)>** result) const;
	void updateListModel(const AbstractListModel* model);

	DemoScene* getScene() const;
	ObjectHandle createObject(Vector3 pos);
	void selectObject(int index);
	void selectScene(const std::string& scene);
	void undoCreateObject(Variant params, Variant result);
	void redoCreateObject(Variant params, Variant result);

	virtual const char* getEnvComponentId() const override;
	virtual void onPostEnvironmentChanged(const EnvironmentId& oldId, const EnvironmentId& newId) override;

	bool initialised_ = false;
	int selectedIndex_ = -1;
	std::string selectedScene_;
	std::map<std::string, ManagedObject<DemoScene>> scenes_;
	const IClassDefinition* definition_ = nullptr;
	
	const AbstractTreeModel* treeModel_;
	Signal<void(Variant&)> treeModelChanged_;

	const AbstractListModel* listModel_;
	Signal<void(Variant&)> listModelChanged_;
};
} // end namespace wgt
#endif // DEMO_OBJECTS_HPP
