#ifndef I_MODEL_EXTENSION_HPP
#define I_MODEL_EXTENSION_HPP

#include "core_variant/variant.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_data_model/abstract_item_model.hpp"
#include "core_common/signal.hpp"

#include <cassert>
#include <vector>
#include <string>
#include <functional>

namespace wgt
{
/** A user extension to a view.
An extension provides additional functionality to a model, by adding additional roles.
Any roles defined in this extension are exposed as additional properties in QML.*/
class IModelExtension
{
public:
	enum class Orientation
	{
		HORIZONTAL,
		VERTICAL
	};

	enum class LayoutHint
	{
		NO_SORT,
		VERTICAL_SORT,
		HORIZONTAL_SORT
	};

	struct IModelIndexData
	{
		virtual int row() const = 0;
		virtual int column() const = 0;
		virtual void* pointer() const = 0;
		virtual const void* model() const = 0;
	};

	struct ModelIndex
	{
		ModelIndex(const std::shared_ptr<IModelIndexData>& data = nullptr)
		{
			data_ = data;
		}

		bool isValid() const
		{
			return row() >= 0 && column() >= 0 && model() != nullptr;
		}

		bool operator==(const ModelIndex& other) const
		{
			return (!isValid() && !other.isValid()) || (row() == other.row() && column() == other.column() &&
			                                            pointer() == other.pointer() && model() == other.model());
		}

		bool operator!=(const ModelIndex& other) const
		{
			return !this->operator==(other);
		}

		int row() const
		{
			if (!data_)
			{
				return -1;
			}

			return data_->row();
		}

		int column() const
		{
			if (!data_)
			{
				return -1;
			}

			return data_->column();
		}

		void* pointer() const
		{
			if (!data_)
			{
				return nullptr;
			}

			return data_->pointer();
		}

		const void* model() const
		{
			if (!data_)
			{
				return nullptr;
			}

			return data_->model();
		}

		std::shared_ptr<IModelIndexData> data() const
		{
			return data_;
		}

	protected:
		std::shared_ptr<IModelIndexData> data_;
	};

	struct IExtensionData
	{
		virtual Variant data(const ModelIndex& index, ItemRole::Id roleId) const = 0;
		virtual bool setData(const ModelIndex& index, const Variant& value, ItemRole::Id roleId) = 0;
		virtual void dataChanged(const ModelIndex& from, const ModelIndex& to,
		                         std::vector<ItemRole::Id> roleIds) const = 0;
		virtual Variant headerData(int section, Orientation orientation, ItemRole::Id roleId) const = 0;
		virtual bool setHeaderData(int section, Orientation orientation, const Variant& value, ItemRole::Id roleId) = 0;
		virtual void headerDataChanged(Orientation orientation, int from, int to) const = 0;
	};

	IModelExtension();
	virtual ~IModelExtension();

	void setExtensionData(IExtensionData* extensionData);

	/** Returns all roles used by the extension.
	@return A vector of role names.*/
	const std::vector<std::string>& roles() const;

	/** Get role data at an index position.
	@param index The position the data applies to.
	@param role The decoded role identifier.
	@return The role data.*/
	virtual Variant data(const ModelIndex& index, ItemRole::Id roleId) const;

	/** Set role data at an index position.
	@param index The position the data applies to.
	@param value The role data.
	@param role The decoded role identifier.
	@return True if successful.*/
	virtual bool setData(const ModelIndex& index, const Variant& value, ItemRole::Id roleId);

	/** Get role data for a header row or column.
	@param section The row or column number.
	@param orientation Specifies whether section refers to a row or column.
	@param role The decoded role identifier.
	@return The role data.*/
	virtual Variant headerData(int section, Orientation orientation, ItemRole::Id roleId) const;

	/** Set role data for a header row or column.
	@param section The row or column number.
	@param orientation Specifies whether section refers to a row or column.
	@param value The role data.
	@param role The decoded role identifier.
	@return True if successful.*/
	virtual bool setHeaderData(int section, Orientation orientation, const Variant& value, ItemRole::Id roleId);

	virtual void onDataChanged(const ModelIndex& topLeft, const ModelIndex& bottomRight,
	                           const std::vector<ItemRole::Id>& roles);
	virtual void onHeaderDataChanged(Orientation orientation, int first, int last);
	virtual void onLayoutAboutToBeChanged(const std::vector<ModelIndex>& parents, LayoutHint hint);
	virtual void onLayoutChanged(const std::vector<ModelIndex>& parents, LayoutHint hint);
	virtual void onRowsAboutToBeInserted(const ModelIndex& parent, int first, int last);
	virtual void onRowsInserted(const ModelIndex& parent, int first, int last);
	virtual void onRowsAboutToBeRemoved(const ModelIndex& parent, int first, int last);
	virtual void onRowsRemoved(const ModelIndex& parent, int first, int last);
	virtual void onRowsAboutToBeMoved(const ModelIndex& sourceParent, int sourceFirst, int sourceLast,
	                                  const ModelIndex& destinationParent, int destinationRow);
	virtual void onRowsMoved(const ModelIndex& sourceParent, int sourceFirst, int sourceLast,
	                         const ModelIndex& destinationParent, int destinationRow);
	virtual void onColumnsAboutToBeInserted(const ModelIndex& parent, int first, int last);
	virtual void onColumnsInserted(const ModelIndex& parent, int first, int last);
	virtual void onColumnsAboutToBeRemoved(const ModelIndex& parent, int first, int last);
	virtual void onColumnsRemoved(const ModelIndex& parent, int first, int last);
	virtual void onColumnsAboutToBeMoved(const ModelIndex& sourceParent, int sourceFirst, int sourceLast,
	                                     const ModelIndex& destinationParent, int destinationRow);
	virtual void onColumnsMoved(const ModelIndex& sourceParent, int sourceFirst, int sourceLast,
	                            const ModelIndex& destinationParent, int destinationColumn);

protected:
	std::vector<std::string> roles_;
	IExtensionData* extensionData_;
};
} // end namespace wgt

namespace std
{
template <>
struct hash<wgt::IModelExtension::ModelIndex>
{
	typedef wgt::IModelExtension::ModelIndex argument_type;
	typedef size_t result_type;

	result_type operator()(argument_type const& i) const
	{
		result_type temp =
		result_type(i.model()) + result_type(i.pointer()) + result_type(i.row()) + result_type(i.column());
		return static_cast<size_t>(wgt::HashUtilities::compute(static_cast<uint64_t>(temp)));
	}
};
}
#endif // I_MODEL_EXTENSION_HPP
