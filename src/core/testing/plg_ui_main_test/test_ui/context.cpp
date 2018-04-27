#include "context.hpp"
#include "pages/test_page.hpp"
#include "core_data_model/reflection_proto/property_tree_model.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_environment_system/i_env_system.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include <cassert>
#include <string>

namespace wgt
{
const std::string NO_ID("NO_ID");
ITEMROLE(value)

TestUIContext::TestUIContext()
{
}

TestUIContext::~TestUIContext()
{
}

void TestUIContext::initialise(ObjectHandle model)
{
	auto definitionManager = get<IDefinitionManager>();
	definition_ = definitionManager->getDefinition<TestUIContext>();
	assert(definition_);

	selectedModel_ = model;
	selectedTree_ = std::make_shared<proto::PropertyTreeModel>(selectedModel_);
	selectedId_ = NO_ID;
}

void TestUIContext::open(std::string id, ObjectHandle model)
{
	assert(models_.find(id) == models_.end());
	models_[id] = model;
	select(id);
}

void TestUIContext::close(std::string id)
{
	if (id == selectedId_)
	{
		select(NO_ID);
	}

	assert(models_.find(id) != models_.end());
	models_.erase(id);
}

void TestUIContext::select(std::string id)
{
	std::string oldId = selectedId_;
	selectedId_ = models_.find(id) != models_.end() ? id : NO_ID;
	treeVisible_ = selectedId_ != NO_ID;
	selectedModel_ = treeVisible_ ? models_.at(selectedId_) : ObjectHandle();
	definition_->bindProperty("treeVisible", handle()).setValue(treeVisible_);

	if (oldId != selectedId_)
	{
		auto tree = std::static_pointer_cast<proto::PropertyTreeModel>(selectedTree_);
		tree->setObject(selectedModel_);
	}
}

AbstractTreeModel* TestUIContext::getTreeModel()
{
	return selectedTree_.get();
}

void TestUIContext::reloadEnvironment()
{
	get<IEnvManager>()->resetCurrentEnvironment();
}

void TestUIContext::toggleReadOnly()
{
	TestPage::toggleReadOnly();
}

void TestUIContext::updateValues()
{
	if (selectedId_ == NO_ID)
	{
		return;
	}

	auto model = getTreeModel();

	const AbstractTreeModel::ItemIndex rootIndex(0, nullptr);
	auto pRoot = model->item(rootIndex);
	const AbstractTreeModel::ItemIndex simpleCasesIndex(0, pRoot);
	auto pSimpleCases = model->item(simpleCasesIndex);

	const AbstractTreeModel::ItemIndex boolIndex(0, pRoot);
	auto pBool = model->item(boolIndex);
	if (pBool != nullptr)
	{
		const auto oldVariant = pBool->getData(0, 0, ItemRole::valueId);
		bool oldBool = false;
		const auto castOk = oldVariant.tryCast(oldBool);
		assert(castOk);
		const auto setOk = pBool->setData(0, 0, ItemRole::valueId, !oldBool);
		assert(setOk);
	}

	const AbstractTreeModel::ItemIndex checkBoxIndex(1, pRoot);
	auto pCheckBox = model->item(checkBoxIndex);
	if (pCheckBox != nullptr)
	{
		const auto oldVariant = pCheckBox->getData(0, 0, ItemRole::valueId);
		bool oldBool = false;
		const auto castOk = oldVariant.tryCast(oldBool);
		assert(castOk);
		const auto setOk = pCheckBox->setData(0, 0, ItemRole::valueId, !oldBool);
		assert(setOk);
	}

	const AbstractTreeModel::ItemIndex slideIndex(3, pRoot);
	auto pSlide = model->item(slideIndex);
	if (pSlide != nullptr)
	{
		const auto oldVariant = pSlide->getData(0, 0, ItemRole::valueId);
		float oldfloat = -1.0f;
		const auto castOk = oldVariant.tryCast(oldfloat);
		assert(castOk);
		const auto setOk = pSlide->setData(0, 0, ItemRole::valueId, oldfloat + 1.0f);
		assert(setOk);
	}

	const AbstractTreeModel::ItemIndex numberIndex(4, pRoot);
	auto pNumber = model->item(numberIndex);
	if (pNumber != nullptr)
	{
		const auto oldVariant = pNumber->getData(0, 0, ItemRole::valueId);
		int oldInt = 1;
		const auto castOk = oldVariant.tryCast(oldInt);
		assert(castOk);
		const auto setOk = pNumber->setData(0, 0, ItemRole::valueId, oldInt + 1);
		assert(setOk);
	}

	const AbstractTreeModel::ItemIndex textFieldIndex(2, pRoot);
	auto pTextField = model->item(textFieldIndex);
	if ((pTextField != nullptr) && (pNumber != nullptr))
	{
		const auto oldVariant = pNumber->getData(0, 0, ItemRole::valueId);
		int newInt = 1;
		const auto castOk = oldVariant.tryCast(newInt);
		assert(castOk);
		std::string newString("Hello Test");
		newString.append(std::to_string(newInt));
		const auto setOk = pTextField->setData(0, 0, ItemRole::valueId, newString);
		assert(setOk);
	}
}

void TestUIContext::undoUpdateValues(Variant, Variant)
{
	// values automatically undone
}

void TestUIContext::redoUpdateValues(Variant, Variant)
{
	// values automatically redone
}

} // end namespace wgt
