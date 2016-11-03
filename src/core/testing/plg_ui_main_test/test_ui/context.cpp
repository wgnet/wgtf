#include "context.hpp"
#include "metadata/context.mpp"

#include "core_data_model/abstract_item_model.hpp"
#include "core_data_model/i_item_role.hpp"

#include <cassert>
#include <string>

namespace wgt
{
ITEMROLE(value)

TestUIContext::TestUIContext()
{
}

TestUIContext::~TestUIContext()
{
}

void TestUIContext::initialize(std::unique_ptr<AbstractTreeModel>&& model)
{
	model_ = std::move(model);
}

AbstractTreeModel* TestUIContext::treeModel() const
{
	return model_.get();
}

void TestUIContext::updateValues()
{
	assert(model_ != nullptr);
	const AbstractTreeModel::ItemIndex rootIndex(0, nullptr);
	auto pRoot = model_->item(rootIndex);
	const AbstractTreeModel::ItemIndex simpleCasesIndex(0, pRoot);
	auto pSimpleCases = model_->item(simpleCasesIndex);

	const AbstractTreeModel::ItemIndex boolIndex(0, pRoot);
	auto pBool = model_->item(boolIndex);
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
	auto pCheckBox = model_->item(checkBoxIndex);
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
	auto pSlide = model_->item(slideIndex);
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
	auto pNumber = model_->item(numberIndex);
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
	auto pTextField = model_->item(textFieldIndex);
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

void TestUIContext::undoUpdateValues(const ObjectHandle&, Variant)
{
	// values automatically undone
}

void TestUIContext::redoUpdateValues(const ObjectHandle&, Variant)
{
	// values automatically redone
}

} // end namespace wgt
