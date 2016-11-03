#ifndef GRID_EDITOR_TEST_DATA_HPP
#define GRID_EDITOR_TEST_DATA_HPP

#include "core_reflection/object_handle.hpp"
#include "core_reflection/reflected_object.hpp"
#include "wg_types/vector2.hpp"
#include "wg_types/vector3.hpp"
#include "wg_types/vector4.hpp"
#include <string>
#include <vector>

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

class GridEditorTestObject1
{
public:
	int intProperty_;
	float floatProperty_;
	std::string stringProperty_;
	std::vector<int> collectionProperty_;
	ObjectHandleT<GridEditorTestStruct> structProperty_;
};

class GridEditorTestObject2
{
public:
	int enumProperty_;
	std::string stringProperty_;
	std::vector<int> collectionProperty_;
};

class GridEditorTestObject3
{
public:
	float sliderProperty_;
	std::string stringProperty_;
	ObjectHandleT<GridEditorTestStruct> structProperty_;
};

class GridEditorTestModel
{
	DECLARE_REFLECTED

public:
	void init(IComponentContext& context);

private:
	std::vector<ObjectHandleT<AbstractTreeModel>> objects_;
};
}

#endif //GRID_EDITOR_TEST_DATA_HPP
