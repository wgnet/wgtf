#pragma once

#include "core_reflection/object_handle.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_object/managed_object.hpp"
#include "wg_types/vector2.hpp"
#include "wg_types/vector3.hpp"
#include "wg_types/vector4.hpp"
#include <string>
#include <vector>

namespace wgt
{
class AbstractTreeModel;
class IComponentContext;

struct MultiEditTestStruct
{
	Vector2 vector2Property_;
	Vector3 vector3Property_;
	Vector4 colorProperty_;
	Vector4 hdrcolorProperty_;
};

class MultiEditTestObject1
{
public:
	bool boolProperty_;
	int intProperty_;
	float floatProperty_;
	std::string stringProperty_;
	std::vector<int> collectionProperty_;
    ManagedObject<MultiEditTestStruct> structObj_;
    ObjectHandleT<MultiEditTestStruct> structProperty_;
	std::string fileUrl_;
};

class MultiEditTestObject2
{
public:
	bool boolProperty_;
	int intProperty_;
	float floatProperty_;
	int enumProperty_;
	std::string stringProperty_;
	std::vector<int> collectionProperty_;
	std::string fileUrl_;
};

class MultiEditTestObject3
{
public:
	bool boolProperty_;
	int intProperty_;
	float floatProperty_;
	int enumProperty_;
	float sliderProperty_;
	std::string stringProperty_;
    ManagedObject<MultiEditTestStruct> structObj_;
    ObjectHandleT<MultiEditTestStruct> structProperty_;
	std::string fileUrl_;
};

class MultiEditTestModel
{
	DECLARE_REFLECTED

public:
	void init(IComponentContext& context);
	void fini();

private:
	std::vector<ManagedObjectPtr> data_;
	std::vector<std::shared_ptr<AbstractTreeModel>> trees_;
	std::vector<AbstractTreeModel*> objects_;
};
}
