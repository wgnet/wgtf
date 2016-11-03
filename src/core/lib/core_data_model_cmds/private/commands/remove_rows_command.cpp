#include "remove_rows_command.hpp"
#include "remove_rows_command_arg.hpp"

#include "core_command_system/i_command_manager.hpp"
#include "core_data_model/abstract_item_model.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_variant/variant.hpp"

namespace wgt
{
namespace RemoveRowsCommand_Detail
{
bool isValid(const RemoveRowsCommandArgument* pCommandArgs)
{
	if (pCommandArgs == nullptr)
	{
		return false;
	}

	if (pCommandArgs->pModel_ == nullptr)
	{
		return false;
	}

	if (pCommandArgs->startPos_ < 0)
	{
		return false;
	}

	if (pCommandArgs->count_ < 1)
	{
		return false;
	}

	return true;
}

/**
 *	Check that index to be set on the model is within the range of [startPos, count].
 */
bool isIndexInRange(
int startPos,
RemoveRowsCommandArgument::Type type,
int count,
const AbstractItemModel::ItemIndex& index)
{
	if (type == RemoveRowsCommandArgument::Type::ROW)
	{
		if (index.row_ < startPos)
		{
			return false;
		}
		if (index.row_ > (startPos + count))
		{
			return false;
		}
	}
	else
	{
		if (index.column_ < startPos)
		{
			return false;
		}
		if (index.column_ > (startPos + count))
		{
			return false;
		}
	}
	return true;
}

/**
 *	Remove empty rows/columns and then set the row/column data back on it.
 */
bool restoreRows(
int startPos,
RemoveRowsCommandArgument::Type type,
int count,
const AbstractItem* pParent,
const RemoveRowsCommandArgument::ExtractedRowsStorage& itemData,
AbstractItemModel& o_model)
{
	// Remove empty row/column
	if (type == RemoveRowsCommandArgument::Type::ROW)
	{
		const auto inserted = o_model.insertRows(startPos, count, pParent);
		if (!inserted)
		{
			return false;
		}
	}
	else
	{
		const auto inserted = o_model.insertColumns(startPos, count, pParent);
		if (!inserted)
		{
			return false;
		}
	}

	// Restore the row/column's contents
	bool setOk = true;
	for (const auto& extractedRows : itemData)
	{
		assert(isIndexInRange(startPos,
		                      type,
		                      count,
		                      extractedRows.index_));
		const auto pItem = o_model.item(extractedRows.index_);
		assert(pItem != nullptr);

		Collection collection;
		if (extractedRows.data_.tryCast(collection))
		{
			auto innerModel = pItem->getData(extractedRows.index_.row_,
			                                 extractedRows.index_.column_,
			                                 extractedRows.roleId_);
			ObjectHandle handle;
			innerModel.tryCast(handle);
			auto pInnerModel = handle.getBase<AbstractItemModel>();
			const auto pInnerRows = collection.container<
			RemoveRowsCommandArgument::ExtractedRowsStorage>();
			setOk &= (pInnerModel != nullptr) && (pInnerRows != nullptr);
			if (!setOk)
			{
				return false;
			}

			// Recursively restore
			const int innerStartPos = 0;
			const int innerCount = static_cast<int>(collection.size());
			const AbstractItem* pInnerParent = nullptr;

			setOk &= RemoveRowsCommand_Detail::restoreRows(innerStartPos,
			                                               type,
			                                               count,
			                                               pParent,
			                                               (*pInnerRows),
			                                               (*pInnerModel));
		}
		else
		{
			setOk &= pItem->setData(extractedRows.index_.row_,
			                        extractedRows.index_.column_,
			                        extractedRows.roleId_,
			                        extractedRows.data_);
		}
	}
	return setOk;
}

} // end namespace RemoveRowsCommand_Detail

RemoveRowsCommand::RemoveRowsCommand(IComponentContext& context)
    : definitionManager_(context)
{
}

bool RemoveRowsCommand::customUndo() const /* override */
{
	return true;
}

bool RemoveRowsCommand::canUndo(const ObjectHandle& arguments) const /* override */
{
	return this->validateArguments(arguments);
}

bool RemoveRowsCommand::undo(const ObjectHandle& arguments) const /* override */
{
	if (!arguments.isValid())
	{
		return false;
	}

	auto pCommandArgs = arguments.getBase<RemoveRowsCommandArgument>();
	if (!RemoveRowsCommand_Detail::isValid(pCommandArgs))
	{
		return false;
	}

	auto& model = (*pCommandArgs->pModel_);
	const auto& startPos = (pCommandArgs->startPos_);
	const auto& type = (pCommandArgs->type_);
	const auto& count = (pCommandArgs->count_);
	const auto& pParent = (pCommandArgs->pParent_);
	const auto& itemData = (pCommandArgs->itemData_);

	const bool inserted = RemoveRowsCommand_Detail::restoreRows(startPos,
	                                                            type,
	                                                            count,
	                                                            pParent,
	                                                            itemData,
	                                                            model);
	assert(inserted && "Item data was not restored correctly");
	return inserted;
}

bool RemoveRowsCommand::redo(const ObjectHandle& arguments) const /* override */
{
	if (!arguments.isValid())
	{
		return false;
	}

	auto pCommandArgs = arguments.getBase<RemoveRowsCommandArgument>();
	if (!RemoveRowsCommand_Detail::isValid(pCommandArgs))
	{
		return false;
	}

	auto& model = (*pCommandArgs->pModel_);
	const auto& startPos = (pCommandArgs->startPos_);
	const auto& type = (pCommandArgs->type_);
	const auto& count = (pCommandArgs->count_);
	const auto& pParent = (pCommandArgs->pParent_);

	if (type == RemoveRowsCommandArgument::Type::ROW)
	{
		return model.removeRows(startPos, count, pParent);
	}
	return model.removeColumns(startPos, count, pParent);
}

ObjectHandle RemoveRowsCommand::getCommandDescription(
const ObjectHandle& arguments) const /* override */
{
	auto handle = GenericObject::create(*definitionManager_);
	assert(handle.get() != nullptr);
	auto& genericObject = (*handle);

	if (!arguments.isValid())
	{
		genericObject.set("Name", "Invalid");
		genericObject.set("Type", "Remove");
		return ObjectHandle(std::move(handle));
	}

	auto pCommandArgs = arguments.getBase<RemoveRowsCommandArgument>();
	if (!RemoveRowsCommand_Detail::isValid(pCommandArgs))
	{
		genericObject.set("Name", "Invalid");
		genericObject.set("Type", "Remove");
		return ObjectHandle(std::move(handle));
	}

	genericObject.set("Name", "Remove");
	genericObject.set("Type", "Remove");

	return ObjectHandle(std::move(handle));
}

const char* RemoveRowsCommand::getId() const /* override */
{
	static const char* s_Id = wgt::getClassIdentifier<RemoveRowsCommand>();
	return s_Id;
}

bool RemoveRowsCommand::validateArguments(
const ObjectHandle& arguments) const /* override */
{
	const auto pCommandArgs = arguments.getBase<RemoveRowsCommandArgument>();
	return RemoveRowsCommand_Detail::isValid(pCommandArgs);
}

ObjectHandle RemoveRowsCommand::execute(
const ObjectHandle& arguments) const /* override */
{
	auto pCommandArgs = arguments.getBase<RemoveRowsCommandArgument>();
	if (!RemoveRowsCommand_Detail::isValid(pCommandArgs))
	{
		return CommandErrorCode::INVALID_ARGUMENTS;
	}

	auto& model = (*pCommandArgs->pModel_);
	const auto& startPos = (pCommandArgs->startPos_);
	const auto& type = (pCommandArgs->type_);
	const auto& count = (pCommandArgs->count_);
	const auto& pParent = (pCommandArgs->pParent_);
	pCommandArgs->saveRows();

	auto result = false;
	if (type == RemoveRowsCommandArgument::Type::ROW)
	{
		result = model.removeRows(startPos, count, pParent);
	}
	else
	{
		result = model.removeColumns(startPos, count, pParent);
	}

	const auto errorCode = result ?
	CommandErrorCode::COMMAND_NO_ERROR :
	CommandErrorCode::FAILED;
	return errorCode;
}

CommandThreadAffinity RemoveRowsCommand::threadAffinity() const /* override */
{
	return CommandThreadAffinity::UI_THREAD;
}

} // end namespace wgt
