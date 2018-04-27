#include "multi_edit_test_data.hpp"
#include "core_data_model/reflection_proto/reflected_tree_model.hpp"
#include "core_common/platform_path.hpp"
#include "core_common/platform_dll.hpp"

namespace wgt
{
void MultiEditTestModel::init(IComponentContext& context)
{
	data_.push_back(ManagedObject<MultiEditTestObject1>::make_unique());
	data_.push_back(ManagedObject<MultiEditTestObject2>::make_unique());
	data_.push_back(ManagedObject<MultiEditTestObject3>::make_unique());
	data_.push_back(ManagedObject<MultiEditTestObject3>::make_unique());

	wchar_t path[MAX_PATH];
	::GetModuleFileNameW(NULL, path, MAX_PATH);
	::PathRemoveFileSpecW(path);
	::PathAppendW(path, L"plugins\\");

	// std::wstring_convert is reported as memory leak - it does custom dtor call and free for codecvt object
	std::unique_ptr<char[]> str(new char[2 * MAX_PATH]);
	wcstombs(str.get(), path, 2 * MAX_PATH);

	auto testObject1 = data_[0]->getHandleT<MultiEditTestObject1>();
	testObject1->boolProperty_ = true;
	testObject1->intProperty_ = 123;
	testObject1->floatProperty_ = 12345.6789f;
	testObject1->stringProperty_ = "Not Checked Out";
	testObject1->collectionProperty_.push_back(0);
	testObject1->collectionProperty_.push_back(1);
	testObject1->collectionProperty_.push_back(2);
	testObject1->collectionProperty_.push_back(3);
	testObject1->collectionProperty_.push_back(4);
	testObject1->structObj_ = ManagedObject<MultiEditTestStruct>::make();
    testObject1->structProperty_ = testObject1->structObj_.getHandleT();
	testObject1->structProperty_->vector2Property_ = Vector2(123.f, 456.f);
	testObject1->structProperty_->vector3Property_ = Vector3(123.f, 456.f, 789.f);
	testObject1->structProperty_->colorProperty_ = Vector4(255.f, 255.f, 255.f, 255.f);
	testObject1->structProperty_->hdrcolorProperty_ = Vector4(0.5f, 0.5f, 0.5f, 0.5f);
	testObject1->fileUrl_ = str.get();
	testObject1->fileUrl_ += "plugins_ui.txt";

	auto testObject2 = data_[1]->getHandleT<MultiEditTestObject2>();
	testObject2->boolProperty_ = true;
	testObject2->intProperty_ = 123;
	testObject2->floatProperty_ = 12345.6789f;
	testObject2->enumProperty_ = 0;
	testObject2->stringProperty_ = "Checked Out";
	testObject2->collectionProperty_.push_back(9);
	testObject2->collectionProperty_.push_back(8);
	testObject2->collectionProperty_.push_back(7);
	testObject2->collectionProperty_.push_back(6);
	testObject2->collectionProperty_.push_back(5);
	testObject2->fileUrl_ = str.get();
	testObject2->fileUrl_ += "plugins_ui.txt";

	auto testObject3 = data_[2]->getHandleT<MultiEditTestObject3>();
	testObject3->boolProperty_ = false;
	testObject3->intProperty_ = 125;
	testObject3->floatProperty_ = 2345.6789f;
	testObject3->enumProperty_ = 0;
	testObject3->sliderProperty_ = 50.f;
	testObject3->stringProperty_ = "Changed in Excel";
    testObject3->structObj_ = ManagedObject<MultiEditTestStruct>::make();
    testObject3->structProperty_ = testObject3->structObj_.getHandleT();
	testObject3->structProperty_->vector2Property_ = Vector2(789.f, 456.f);
	testObject3->structProperty_->vector3Property_ = Vector3(789.f, 456.f, 123.f);
	testObject3->structProperty_->colorProperty_ = Vector4(0.f, 0.f, 0.f, 0.f);
	testObject1->structProperty_->hdrcolorProperty_ = Vector4(0.5f, 0.5f, 0.5f, 0.5f);
	testObject3->fileUrl_ = str.get();
	testObject3->fileUrl_ += "plugins_ui_mac.txt";

	auto testObject4 = data_[3]->getHandleT<MultiEditTestObject3>();
	testObject4->boolProperty_ = false;
	testObject4->intProperty_ = 125;
	testObject4->floatProperty_ = 2345.6789f;
	testObject4->enumProperty_ = 1;
	testObject4->sliderProperty_ = 80.f;
	testObject4->stringProperty_ = "Changed in Excel & Checked Out";
    testObject4->structObj_ = ManagedObject<MultiEditTestStruct>::make();
    testObject4->structProperty_ = testObject4->structObj_.getHandleT();
	testObject4->structProperty_->vector2Property_ = Vector2(289.f, 156.f);
	testObject4->structProperty_->vector3Property_ = Vector3(289.f, 156.f, 123.f);
	testObject4->structProperty_->colorProperty_ = Vector4(0.f, 0.f, 0.f, 0.f);
	testObject4->structProperty_->hdrcolorProperty_ = Vector4(0.f, 0.f, 0.f, 1.f);

	for (const auto& data : data_)
	{
		trees_.push_back(std::make_unique<proto::ReflectedTreeModel>(data->getHandle()));
		objects_.push_back(trees_.back().get());
	}
}

void MultiEditTestModel::fini()
{
	objects_.clear();
}
}
