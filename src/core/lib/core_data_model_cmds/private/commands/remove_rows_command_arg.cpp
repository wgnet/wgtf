#include "remove_rows_command_arg.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_variant/collection.hpp"

namespace wgt
{
namespace RemoveRowsCommandArgument_Detail
{
/**
 *	Get state from rows/columns and save it into a struct.
 */
void extractRows(const AbstractItemModel& model,
                 const AbstractItemModel::ItemIndex& startIndex,
                 RemoveRowsCommandArgument::Type type,
                 int count,
                 RemoveRowsCommandArgument::ExtractedRowsStorage& o_data)
{
	std::vector<ItemRole::Id> roleIds;
	{
		const auto roleNames = model.roles();
		roleIds.reserve(roleNames.size());
		for (const auto& roleName : roleNames)
		{
			roleIds.emplace_back(ItemRole::compute(roleName.c_str()));
		}
	}

	AbstractItemModel::ItemIndex index(startIndex);

	// Iterate through given range of rows (RemoveType::ROW)
	// otherwise iterating columns
	for (int row = 0; row < count; ++row)
	{
		// Iterate through all columns for the row (RemoveType::ROW)
		// otherwise iterating rows
		int columnCount = 0;
		if (type == RemoveRowsCommandArgument::Type::ROW)
		{
			columnCount = model.columnCount(startIndex.parent_);
		}
		else
		{
			columnCount = model.rowCount(startIndex.parent_);
		}

		for (int column = 0; column < columnCount; ++column)
		{
			const auto pItem = model.item(index);
			assert(pItem != nullptr);
			for (const auto& roleId : roleIds)
			{
				RemoveRowsCommandArgument::ExtractedRows extractedRows;
				extractedRows.data_ = pItem->getData(index.row_,
				                                     index.column_,
				                                     roleId);

				if (!extractedRows.data_.isVoid())
				{
					ObjectHandle handle;
					extractedRows.data_.tryCast(handle);
					auto pInnerModel = handle.getBase<AbstractItemModel>();
					if (pInnerModel != nullptr)
					{
						// Recursively extract
						const int innerRow = 0;
						const int innerColumn = 0;
						const AbstractItem* pInnerParent = nullptr;
						const AbstractItemModel::ItemIndex innerStartIndex(
						innerRow,
						innerColumn,
						pInnerParent);
						const int innerCount = pInnerModel->rowCount(pInnerParent);

						// Need to create a CollectionHolder, otherwise
						// extractedRows.data_ = innerData;
						// is unsafe, because it takes a reference
						// which will be deleted when innerData goes out of scope
						auto collectionHolder = std::make_shared<CollectionHolder<
						RemoveRowsCommandArgument::ExtractedRowsStorage>>();
						auto& innerData = collectionHolder->storage();
						innerData.reserve(innerCount);

						extractRows((*pInnerModel),
						            innerStartIndex,
						            type,
						            innerCount,
						            innerData);

						extractedRows.data_ = Collection(collectionHolder);
					}

					extractedRows.index_ = index;
					extractedRows.roleId_ = roleId;
					o_data.emplace_back(extractedRows);
				}
			}

			if (type == RemoveRowsCommandArgument::Type::ROW)
			{
				++index.column_;
			}
			else
			{
				++index.row_;
			}
		}

		if (type == RemoveRowsCommandArgument::Type::ROW)
		{
			++index.row_;
		}
		else
		{
			++index.column_;
		}
	}
}

} // end namespace RemoveRowsCommandArgument_Detail

RemoveRowsCommandArgument::RemoveRowsCommandArgument()
    : pModel_(nullptr)
    , startPos_(-1)
    , type_(Type::ROW)
    , count_(-1)
    , pParent_(nullptr)
{
}

void RemoveRowsCommandArgument::setModel(AbstractItemModel& model)
{
	pModel_ = &model;
}

void RemoveRowsCommandArgument::setStartPos(int startPos, Type type)
{
	startPos_ = startPos;
	type_ = type;
}

void RemoveRowsCommandArgument::setCount(int count)
{
	count_ = count;
}

void RemoveRowsCommandArgument::setParent(const AbstractItem* pParent)
{
	pParent_ = pParent;
}

void RemoveRowsCommandArgument::saveRows()
{
	assert(pModel_ != nullptr);

	const AbstractItemModel::ItemIndex startIndex(
	type_ == Type::ROW ? startPos_ : 0,
	type_ == Type::COLUMN ? startPos_ : 0,
	pParent_);
	RemoveRowsCommandArgument_Detail::extractRows((*pModel_),
	                                              startIndex,
	                                              type_,
	                                              count_,
	                                              itemData_);
}

} // end namespace wgt
