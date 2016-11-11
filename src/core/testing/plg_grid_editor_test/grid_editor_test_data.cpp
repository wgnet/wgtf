#include "grid_editor_test_data.hpp"
#include "grid_editor_test_data.mpp"
#include "core_data_model/reflection_proto/property_list_model.hpp"

namespace wgt
{
void GridEditorTestModel::init(IComponentContext& context)
{
	auto definitionManager = context.queryInterface<IDefinitionManager>();

	std::unique_ptr<GridEditorTestObject1> testObject1(new GridEditorTestObject1());
	std::unique_ptr<GridEditorTestObject2> testObject2(new GridEditorTestObject2());
	std::unique_ptr<GridEditorTestObject3> testObject3(new GridEditorTestObject3());
	std::unique_ptr<GridEditorTestObject3> testObject4(new GridEditorTestObject3());
	std::unique_ptr<GridEditorTestObject3> testObject5(new GridEditorTestObject3());

	testObject1->intProperty_ = 123;
	testObject1->floatProperty_ = 12345.6789f;
	testObject1->stringProperty_ = "Not Checked Out";
	testObject1->collectionProperty_.push_back(0);
	testObject1->collectionProperty_.push_back(1);
	testObject1->collectionProperty_.push_back(2);
	testObject1->collectionProperty_.push_back(3);
	testObject1->collectionProperty_.push_back(4);
	testObject1->structProperty_ = definitionManager->create<GridEditorTestStruct>();
	testObject1->structProperty_->vector2Property_ = Vector2(123.f, 456.f);
	testObject1->structProperty_->vector3Property_ = Vector3(123.f, 456.f, 789.f);
	testObject1->structProperty_->colorProperty_ = Vector4(255.f, 255.f, 255.f, 255.f);

	testObject2->enumProperty_ = 0;
	testObject2->stringProperty_ = "Checked Out";
	testObject2->collectionProperty_.push_back(9);
	testObject2->collectionProperty_.push_back(8);
	testObject2->collectionProperty_.push_back(7);
	testObject2->collectionProperty_.push_back(6);
	testObject2->collectionProperty_.push_back(5);

	testObject3->sliderProperty_ = 50.f;
	testObject3->stringProperty_ = "Changed in Excel";
	testObject3->structProperty_ = definitionManager->create<GridEditorTestStruct>();
	testObject3->structProperty_->vector2Property_ = Vector2(789.f, 456.f);
	testObject3->structProperty_->vector3Property_ = Vector3(789.f, 456.f, 123.f);
	testObject3->structProperty_->colorProperty_ = Vector4(0.f, 0.f, 0.f, 0.f);

	testObject4->sliderProperty_ = 80.f;
	testObject4->stringProperty_ = "Changed in Excel & Checked Out";
	testObject4->structProperty_ = definitionManager->create<GridEditorTestStruct>();
	testObject4->structProperty_->vector2Property_ = Vector2(289.f, 156.f);
	testObject4->structProperty_->vector3Property_ = Vector3(289.f, 156.f, 123.f);
	testObject4->structProperty_->colorProperty_ = Vector4(0.f, 0.f, 0.f, 0.f);

	testObject5->sliderProperty_ = 80.f;
	testObject5->stringProperty_ = "Changed in Excel";
	testObject5->structProperty_ = definitionManager->create<GridEditorTestStruct>();
	testObject5->structProperty_->vector2Property_ = Vector2(289.f, 156.f);
	testObject5->structProperty_->vector3Property_ = Vector3(289.f, 156.f, 123.f);
	testObject5->structProperty_->colorProperty_ = Vector4(0.f, 0.f, 0.f, 0.f);

	std::unique_ptr<AbstractTreeModel> object1(new proto::PropertyListModel(context, std::move(testObject1)));
	std::unique_ptr<AbstractTreeModel> object2(new proto::PropertyListModel(context, std::move(testObject2)));
	std::unique_ptr<AbstractTreeModel> object3(new proto::PropertyListModel(context, std::move(testObject3)));
	std::unique_ptr<AbstractTreeModel> object4(new proto::PropertyListModel(context, std::move(testObject4)));
	std::unique_ptr<AbstractTreeModel> object5(new proto::PropertyListModel(context, std::move(testObject5)));

	objects_.push_back(std::move(object1));
	objects_.push_back(std::move(object2));
	objects_.push_back(std::move(object3));
	objects_.push_back(std::move(object4));
	objects_.push_back(std::move(object5));
}

void GridEditorTestModel::fini()
{
	objects_.clear();
}
}
