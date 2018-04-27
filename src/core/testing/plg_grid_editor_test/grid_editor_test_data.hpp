#ifndef GRID_EDITOR_TEST_DATA_HPP
#define GRID_EDITOR_TEST_DATA_HPP

#include "core_reflection/object_handle.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_object/managed_object.hpp"
#include "wg_types/vector2.hpp"
#include "wg_types/vector3.hpp"
#include "wg_types/vector4.hpp"
#include <string>
#include <vector>
#include <map>

namespace wgt
{
class AbstractTreeModel;
class IComponentContext;

struct GridEditorTestStruct
{
	Vector2 vector2Property_;
	Vector3 vector3Property_;
	Vector4 colorProperty_;
};

class GridEditorTestObject1 : Depends<IDefinitionManager>
{
public:
	GridEditorTestObject1();
	int intProperty_;
	float floatProperty_;
	std::string stringProperty_;
	std::vector<int> collectionProperty_;
    ManagedObject<GridEditorTestStruct> structObj_;
    ObjectHandleT<GridEditorTestStruct> structProperty_;
};

class GridEditorTestObject2
{
public:
	GridEditorTestObject2();
	int enumProperty_;
	std::string stringProperty_;
	std::vector<int> collectionProperty_;
};

class GridEditorTestObject3 : Depends<IDefinitionManager>
{
public:
	GridEditorTestObject3();
	float sliderProperty_;
	std::string stringProperty_;
	ManagedObject<GridEditorTestStruct> structObj_;
    ObjectHandleT<GridEditorTestStruct> structProperty_;
};

class GridEditorTestModel
{
	DECLARE_REFLECTED

public:
	GridEditorTestModel();
	virtual ~GridEditorTestModel();

	void init();
	void fini();

private:
	GridEditorTestModel(const GridEditorTestModel&);
	GridEditorTestModel& operator = (const GridEditorTestModel&);

    std::vector<ManagedObjectPtr> data_;
    std::vector<std::shared_ptr<AbstractTreeModel>> trees_;
	std::vector<AbstractTreeModel*> objects_;
	int maxObjectCount_ = 0;
};
}

#endif // GRID_EDITOR_TEST_DATA_HPP
