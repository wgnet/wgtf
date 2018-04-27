#include "demo_objects.hpp"

#include "core_automation/interfaces/i_automation.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_data_model/reflection_proto/property_tree_model.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_string_utils/string_utils.hpp"
#include "core_string_utils/file_path.hpp"
#include "testing/reflection_objects_test/test_objects.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_serialization/i_file_system.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_logging/logging.hpp"
#include "wg_types/vector2.hpp"
#include "wg_types/vector4.hpp"
#include <stdio.h>
#include <codecvt>

namespace wgt
{
namespace
{
enum class ModelPropertyValueType : uint8_t
{
	String = 0,
	Boolean,
	Integer,
	Double,
	EnumList,
	Vector2,
	Vector3,
	Vector4,
};

template <typename T>
T getObjectProperty(int index, const std::vector<GenericObjectPtr>& objects, const char* name, T&& defaultValue)
{
	assert(index >= 0 && static_cast<size_t>(index) < objects.size());
	auto genericObject = objects[index];

	const auto accessor = genericObject->findProperty(name);
	if (accessor.isValid())
	{
		T value;
		bool isOk = genericObject->get(name, value);
		assert(isOk);
		return value;
	}

	return std::forward<T>(defaultValue);
}
}

DemoScene::DemoScene(std::function<void(int)> onObjectSelect)
    : objectListModel_(std::make_shared<CollectionModel>()), onObjectSelect_(onObjectSelect)
{
}

DemoScene::~DemoScene()
{
}

void DemoScene::initialise(const std::string& id)
{
    assert(handle() != nullptr);

	definition_ = get<IDefinitionManager>()->getDefinition<DemoScene>();
    assert(definition_);

	id_ = id;
	loadData();
	createTreeModel();
}

void DemoScene::loadData()
{
	std::string file = PROJECT_RESOURCE_FOLDER;
	file += id_;
	file += ".xml";
	if (!get<IFileSystem>()->exists(file.c_str()))
	{
		return; // No data for selected environment
	}

	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(file.c_str()) != tinyxml2::XML_SUCCESS || doc.ErrorID())
	{
		NGT_ERROR_MSG("Failed to load %s\n", file.c_str());
		return;
	}

	auto root = doc.RootElement();
	auto node = root->FirstChildElement("object");
	while (node != nullptr)
	{
		objects_.emplace_back(GenericObject::create());
		auto handle = objects_.back().getHandleT();
		objectHandles_.emplace_back(handle);
		populateDemoObject(handle, *node);
		node = node->NextSiblingElement("object");
	}

	objectListModel_->setSource(Collection(objectHandles_));
}

void DemoScene::populateDemoObject(GenericObjectPtr& genericObject, const tinyxml2::XMLNode& objectNode)
{
	auto propertyNode = objectNode.FirstChildElement("property");
	while (propertyNode != nullptr)
	{
		uint8_t valueType = 0;
		const char* propertyName = propertyNode->Attribute("name");
		auto attribute = propertyNode->FindAttribute("type");
		if (attribute != nullptr)
		{
			valueType = static_cast<uint8_t>(attribute->UnsignedValue());
		}
		std::string tmp("");
		const char* value = propertyNode->GetText();
		if (value != nullptr)
		{
			tmp = value;
		}
		switch (valueType)
		{
		case (uint8_t)ModelPropertyValueType::Boolean:
		{
			bool realValue = false;
			tmp.erase(remove_if(tmp.begin(), tmp.end(), isspace), tmp.end());
			bool isOk = tinyxml2::XMLUtil::ToBool(tmp.c_str(), &realValue);
			assert(isOk);
			genericObject->set(propertyName, realValue);
		}
		break;
		case (uint8_t)ModelPropertyValueType::Integer:
		{
			int realValue = -1;
			tmp.erase(remove_if(tmp.begin(), tmp.end(), isspace), tmp.end());
			bool isOk = tinyxml2::XMLUtil::ToInt(tmp.c_str(), &realValue);
			assert(isOk);
			genericObject->set(propertyName, realValue);
		}
		break;
		case (uint8_t)ModelPropertyValueType::Double:
		{
			double realValue = -1.0;
			tmp.erase(remove_if(tmp.begin(), tmp.end(), isspace), tmp.end());
			bool isOk = tinyxml2::XMLUtil::ToDouble(tmp.c_str(), &realValue);
			assert(isOk);
			genericObject->set(propertyName, realValue);
		}
		break;
		case (uint8_t)ModelPropertyValueType::EnumList:
		{
			auto entries = StringUtils::split(tmp, ',');
			objects_.emplace_back(GenericObject::create());
			auto handle = objects_.back().getHandleT();

			for(int i = 0; i < (int)entries.size(); ++i)
			{
				handle->set(std::to_string(i).c_str(), entries[i]);
			}

			genericObject->set(propertyName, handle);
		}
		break;
		case (uint8_t)ModelPropertyValueType::Vector2:
		{
			tmp.erase(remove_if(tmp.begin(), tmp.end(), isspace), tmp.end());
			float d[2] = { 0.0f, 0.0f };
			char* pch = nullptr;
			pch = strtok(const_cast<char*>(tmp.c_str()), ",");
			int i = 0;
			while (pch != nullptr)
			{
				float v = static_cast<float>(atof(pch));
				d[i++] = v;
				pch = strtok(nullptr, ",");
			}
			assert(i == 2);
			Vector2 realValue(d[0], d[1]);
			genericObject->set(propertyName, realValue);
		}
		break;
		case (uint8_t)ModelPropertyValueType::Vector3:
		{
			tmp.erase(remove_if(tmp.begin(), tmp.end(), isspace), tmp.end());
			float d[3] = { 0.0f, 0.0f, 0.0f };
			char* pch = nullptr;
			pch = strtok(const_cast<char*>(tmp.c_str()), ",");
			int i = 0;
			while (pch != nullptr)
			{
				float v = static_cast<float>(atof(pch));
				d[i++] = v;
				pch = strtok(nullptr, ",");
			}
			assert(i == 3);
			Vector3 realValue(d[0], d[1], d[2]);
			genericObject->set(propertyName, realValue);
		}
		break;
		case (uint8_t)ModelPropertyValueType::Vector4:
		{
			tmp.erase(remove_if(tmp.begin(), tmp.end(), isspace), tmp.end());
			float d[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
			char* pch = nullptr;
			pch = strtok(const_cast<char*>(tmp.c_str()), ",");
			int i = 0;
			while (pch != nullptr)
			{
				float v = static_cast<float>(atof(pch));
				d[i++] = v;
				pch = strtok(nullptr, ",");
			}
			assert(i == 4);
			Vector4 realValue(d[0], d[1], d[2], d[3]);
			genericObject->set(propertyName, realValue);
		}
		break;
		default:
		{
			genericObject->set(propertyName, tmp);
		}
		break;
		}

		propertyNode = propertyNode->NextSiblingElement("property");
	}
}

ObjectHandle DemoScene::createObject(Vector3 pos)
{
	RefObjectId id = RefObjectId::generate();
	ManagedObject<GenericObject> genericObject = GenericObject::create(id);
	GenericObjectPtr genericHandle = genericObject.getHandleT();
	genericHandle->set("name", std::string(" object") + std::to_string(objectHandles_.size() + 1));
	genericHandle->set("position", pos);
	genericHandle->set("rotation", Vector3(0.0f, 0.0f, 0.0f));
	genericHandle->set("scale", Vector3(1.0f, 1.0f, 1.0f));
	genericHandle->set("visible", true);
	genericHandle->set("map1", std::string("logo.png"));

	Collection& objectCollection = objectListModel_->getSource();
	objectCollection.insertValue(objectCollection.size(), genericHandle);
	objects_.emplace_back(std::move(genericObject));

	flagDirty();

	return genericHandle;
}

void DemoScene::undoCreateObject(Variant params, Variant result)
{
	assert(result.canCast<GenericObjectPtr>());
	GenericObjectPtr genericObject = result.cast<GenericObjectPtr>();
	auto it = std::find(objectHandles_.begin(), objectHandles_.end(), genericObject);
	assert(it != objectHandles_.end());
	auto index = it - objectHandles_.begin();
	Collection& objectCollection = objectListModel_->getSource();
	objectCollection.eraseKey(index);

	flagDirty();
}

void DemoScene::redoCreateObject(Variant params, Variant result)
{
	assert(result.canCast<GenericObjectPtr>());
	GenericObjectPtr genericObject = result.cast<GenericObjectPtr>();
	Collection& objectCollection = objectListModel_->getSource();
	objectCollection.insertValue(objectCollection.size(), genericObject);

	flagDirty();
}

bool DemoScene::isSelectedValid() const
{
	return selectedIndex_ >= 0 && selectedIndex_ < (int)objectHandles_.size();
}

void DemoScene::getSceneDirtySignal(Signal<void(Variant&)>** result) const
{
    *result = const_cast<Signal<void(Variant&)>*>(&sceneDirtyChanged_);
}

bool DemoScene::getSceneDirty() const
{
    return sceneDirty_;
}

void DemoScene::createTreeModel()
{
	treeModel_.reset(new proto::PropertyTreeModel(isSelectedValid() ? objectHandles_[selectedIndex_] : nullSelection_));
	AbstractTreeModel::DataCallback onDataChanged = [this](const AbstractTreeModel::ItemIndex&, int, ItemRole::Id,
	                                                       const Variant&) { flagDirty(); };

	treeModel_->connectPostItemDataChanged(onDataChanged);
}

void DemoScene::selectObject(int index)
{
	selectedIndex_ = index;
	definition_->bindProperty("selectedIndex", handle()).setValue(selectedIndex_);
	createTreeModel();
	onObjectSelect_(index);
}

std::string DemoScene::getObjectTexture(int index)
{
	if (index > -1 && index < (int)objectHandles_.size())
	{
		return getObjectProperty<std::string>(index, objectHandles_, "map1", "");
	}
	return "";
}

void DemoScene::setTexture(int index, std::string currFilePath, std::string newFilePath)
{
	if (index > -1 && index < (int)objectHandles_.size())
	{
		objectHandles_[index]->set("map1", newFilePath);
	}

	flagDirty();
}

void DemoScene::undoSetTexture(Variant params, Variant result)
{
	ReflectedMethodParameters args = params.cast<ReflectedMethodParameters>();
	int index = args[0].cast<int>();
	std::string prevFilePath = args[1].cast<std::string>();

	if (index > -1 && index < (int)objectHandles_.size())
	{
		objectHandles_[index]->set("map1", prevFilePath);
	}

	flagDirty();
}

void DemoScene::redoSetTexture(Variant params, Variant result)
{
	ReflectedMethodParameters args = params.cast<ReflectedMethodParameters>();

	int index = args[0].cast<int>();
	std::string newFilePath = args[2].cast<std::string>();

	if (index > -1 && index < (int)objectHandles_.size())
	{
		objectHandles_[index]->set("map1", newFilePath);
	}

	flagDirty();
}

void DemoScene::flagDirty()
{
	sceneDirty_ = true;
    sceneDirtyChanged_(Variant(sceneDirty_));
    sceneDirty_ = false;
    sceneDirtyChanged_(Variant(sceneDirty_));
}

const AbstractTreeModel* DemoScene::getTreeModel() const
{
	return treeModel_.get();
}

const AbstractListModel* DemoScene::getListModel() const
{
	return objectListModel_.get();
}

int DemoScene::selected() const
{
	return selectedIndex_;
}

DemoObjects::DemoObjects(IEnvManager& envManager) : EnvComponentT(envManager)
{
}

DemoObjects::~DemoObjects()
{
}

const char* DemoObjects::getEnvComponentId() const
{
	static const char* s_id = "demoobjects_envcomponentid";
	return s_id;
}

void DemoObjects::onPostEnvironmentChanged(const EnvironmentId& oldId, const EnvironmentId& newId)
{
	if (initialised_)
	{
		selectScene(newId);
	}
}

bool DemoObjects::init(IComponentContext& contextManager)
{
	initEnvComponent();
	
    assert(handle() != nullptr);

    definition_ = get<IDefinitionManager>()->getDefinition<DemoObjects>();
    assert(definition_);

	initialised_ = true;
	return true;
}

bool DemoObjects::fini()
{
	finiEnvComponent();
	initialised_ = false;
	return true;
}

const AbstractTreeModel* DemoObjects::getTreeModel() const
{
	return treeModel_;
}

const AbstractListModel* DemoObjects::getListModel() const
{
	return listModel_;
}

void DemoObjects::getTreeModelSignal(Signal<void(Variant&)>** result) const
{
	*result = const_cast<Signal<void(Variant&)>*>(&treeModelChanged_);
}

void DemoObjects::getListModelSignal(Signal<void(Variant&)>** result) const
{
	*result = const_cast<Signal<void(Variant&)>*>(&listModelChanged_);
}

void DemoObjects::updateTreeModel(const AbstractTreeModel* model)
{
	treeModel_ = model;
	treeModelChanged_(Variant(treeModel_));
}

void DemoObjects::updateListModel(const AbstractListModel* model)
{
	listModel_ = model;
	listModelChanged_(Variant(listModel_));
}

DemoScene* DemoObjects::getScene() const
{
	if (scenes_.find(selectedScene_) == scenes_.end())
	{
		return nullptr;
	}
	return scenes_.at(selectedScene_).getPointer();
}

ObjectHandleT<DemoScene> DemoObjects::createScene(const std::string& id)
{
	auto onSelectObj = [this](int index) {
		selectedIndex_ = index;
		definition_->bindProperty("selectedIndex", handle()).setValue(selectedIndex_);
		updateTreeModel(getScene() ? getScene()->getTreeModel() : nullptr);
	};

	scenes_[id] = ManagedObject<DemoScene>::make(onSelectObj);
	scenes_.at(id)->initialise(id);
	return scenes_.at(id).getHandleT();
}

void DemoObjects::selectScene(const std::string& scene)
{
	if(scene != selectedScene_)
	{
		selectedScene_ = scene;
		definition_->bindProperty("selectedScene", handle()).setValue(selectedScene_);

		auto scene = getScene();
		if(scene)
		{
			selectObject(scene->selected());
			updateListModel(scene->getListModel());
		}
		else
		{
			updateTreeModel(nullptr);
			updateListModel(nullptr);
		}
	}
}

void DemoObjects::selectObject(int index)
{
	auto scene = getScene();
	if (scene)
	{
		scene->selectObject(index);
	}
}

ObjectHandle DemoObjects::createObject(Vector3 pos)
{
	auto scene = getScene();
	if (scene)
	{
		scene->createObject(pos);
	}
	return ObjectHandle();
}

void DemoObjects::undoCreateObject(Variant params, Variant result)
{
	auto scene = getScene();
	if (scene)
	{
		scene->undoCreateObject(params, result);
	}
}

void DemoObjects::redoCreateObject(Variant params, Variant result)
{
	auto scene = getScene();
	if (scene)
	{
		scene->redoCreateObject(params, result);
	}
}

} // end namespace wgt
