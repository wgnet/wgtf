#include "grid_editor_test_data.hpp"
#include "metadata/grid_editor_test_data.mpp"
#include "core_data_model/reflection_proto/property_list_model.hpp"
#include "core_reflection/i_definition_manager.hpp"

namespace wgt
{
GridEditorTestObject1::GridEditorTestObject1()
{
	stringProperty_ = "Test1";
	intProperty_ = 123;
	floatProperty_ = 12345.6789f;
	collectionProperty_.push_back(0);
	collectionProperty_.push_back(1);
	collectionProperty_.push_back(2);
	collectionProperty_.push_back(3);
	collectionProperty_.push_back(4);
    structObj_ = ManagedObject<GridEditorTestStruct>::make();
    structProperty_ = structObj_.getHandleT();
	structProperty_->vector2Property_ = Vector2(123.f, 456.f);
	structProperty_->vector3Property_ = Vector3(123.f, 456.f, 789.f);
	structProperty_->colorProperty_ = Vector4(255.f, 255.f, 255.f, 255.f);
}

GridEditorTestObject2::GridEditorTestObject2()
{
	stringProperty_ = "Test2";
	enumProperty_ = 0;
	collectionProperty_.push_back(9);
	collectionProperty_.push_back(8);
	collectionProperty_.push_back(7);
	collectionProperty_.push_back(6);
	collectionProperty_.push_back(5);
}

GridEditorTestObject3::GridEditorTestObject3()
{
	stringProperty_ = "Test3";
	sliderProperty_ = 50.f;
    structObj_ = ManagedObject<GridEditorTestStruct>::make();
    structProperty_ = structObj_.getHandleT();
	structProperty_->vector2Property_ = Vector2(789.f, 456.f);
	structProperty_->vector3Property_ = Vector3(789.f, 456.f, 123.f);
	structProperty_->colorProperty_ = Vector4(0.f, 0.f, 0.f, 0.f);
}

GridEditorTestModel::GridEditorTestModel()
{
}

GridEditorTestModel::~GridEditorTestModel()
{
}

void GridEditorTestModel::init()
{
	auto createObject = [this](int type)
	{
		switch(type)
		{
		case 1:
            data_.push_back(ManagedObject<GridEditorTestObject1>::make_unique());
			break;
		case 2:
            data_.push_back(ManagedObject<GridEditorTestObject2>::make_unique());
			break;
		case 3:
            data_.push_back(ManagedObject<GridEditorTestObject3>::make_unique());
			break;
        default:
            assert("Unknown data type");
            return;
		}

        trees_.push_back(std::make_unique<proto::PropertyListModel>(data_.back()->getHandle()));
        objects_.push_back(trees_.back().get());
	};

	createObject(1);
	createObject(2);
	createObject(3);
	createObject(1);
	createObject(2);
	createObject(3);
	createObject(1);
	createObject(2);
	createObject(3);

	maxObjectCount_ = (int)objects_.size();
}

void GridEditorTestModel::fini()
{
	objects_.clear();
}

}
