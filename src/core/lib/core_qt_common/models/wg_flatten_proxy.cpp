#include "wg_flatten_proxy.hpp"

#include "core_common/assert.hpp"
#include "core_qt_common/models/wgt_item_model_base.hpp"
#include "core_variant/variant.hpp"

namespace wgt
{

struct WGFlattenProxy::Impl
	: public WgtItemModelBase
{
	struct Mapping
	{
		int size_;
		Mapping* parent_;
		std::vector<std::unique_ptr<Mapping>> children_;

		int getIndex() 
		{
			if (parent_ == nullptr)
			{
				return -1;
			}

			auto index = parent_->getIndex();
			for (auto& sibling : parent_->children_)
			{
				++index;
				if (sibling.get() == this)
				{
					return index;
				}
				index += sibling->size_;
			}
			TF_ASSERT(false);
			return -1;
		}
	};

	Impl(WGFlattenProxy& proxyModel) : proxyModel_(proxyModel)
	{

	}

	~Impl()
	{
		reset();
	}

	QModelIndex proxy_to_source(const QModelIndex& proxyIndex, QAbstractItemModel& sourceModel) const
	{
		if (!proxyIndex.isValid())
		{
			return QModelIndex();
		}

		auto row = proxyIndex.row();
		return row_to_source(row, &mapping_, QModelIndex(), sourceModel);
	}

	QModelIndex source_to_proxy(const QModelIndex& sourceIndex, const WGFlattenProxy& proxyModel)
	{
		if (!sourceIndex.isValid())
		{
			return QModelIndex();
		}

		auto mapping = get_mapping(sourceIndex);
		return proxyModel.index(mapping->getIndex(), 0);
	}

	QModelIndex row_to_source(int row, const Mapping* mapping, const QModelIndex& source_parent, QAbstractItemModel& source_model) const
	{
		for (size_t i = 0; i < mapping->children_.size(); ++i)
		{
			if (row == 0)
			{
				return source_model.index(static_cast<int>(i), 0, source_parent);
			}
			--row;

			auto child_mapping = mapping->children_[i].get();
			if (row < child_mapping->size_)
			{
				auto source_index = source_model.index(static_cast<int>(i), 0, source_parent);
				return row_to_source(row, child_mapping, source_index, source_model);
			}

			row -= child_mapping->size_;
		}
		return QModelIndex();
	}


	void reset()
	{
		mapping_.size_ = 0;
		mapping_.parent_ = nullptr;
		mapping_.children_.clear();
	}

	Mapping* get_mapping(const QModelIndex& source_index)
	{
		if (!source_index.isValid())
		{
			return &mapping_;
		}

		auto source_parent = source_index.parent();
		auto mapping = get_mapping(source_parent);
		TF_ASSERT(static_cast<int>(mapping->children_.size()) > source_index.row());
		return mapping->children_[source_index.row()].get();
	}

	void flatten(QAbstractItemModel& source_model)
	{
		auto rowCount = source_model.rowCount();
		if (rowCount == 0)
		{
			return;
		}

		TF_ASSERT(mapping_.children_.empty());
		flatten(QModelIndex(), 0, rowCount - 1, source_model, mapping_.children_);
		for (auto& child : mapping_.children_)
		{
			child->parent_ = &mapping_;
			mapping_.size_ += child->size_ + 1;
		}
	}

	void flatten(const QModelIndex& source_parent, int first, int last, QAbstractItemModel& source_model, std::vector<std::unique_ptr<Mapping>>& mappings)
	{
		auto count = last - first + 1;

		for (int i = 0; i < count; ++i)
		{
			auto new_mapping = new Impl::Mapping();
			new_mapping->size_ = 0;
			mappings.emplace_back(new_mapping);
		}

		for (int i = 0; i < count; ++i)
		{
			auto mapping = mappings[i].get();
			auto source_index = source_model.index(first + i, 0, source_parent);
			auto source_row_count = source_model.rowCount(source_index);
			flatten(source_index, 0, source_row_count - 1, source_model, mapping->children_);
			for (auto& child : mapping->children_)
			{
				child->parent_ = mapping;
				mapping->size_ += child->size_ + 1;
			}
		}
	}

	void grow(Mapping* mapping, int count)
	{
		if (mapping == nullptr)
		{
			return;
		}

		mapping->size_ += count;
		grow(mapping->parent_, count);
	}

	QAbstractItemModel * getSourceModel() const override
	{
		return proxyModel_.sourceModel();
	}

	QModelIndex getSourceIndex(const QModelIndex & index) const override
	{
		return proxy_to_source(index, *getSourceModel());
	}

	Mapping mapping_;
	QtConnectionHolder connections_;
	WGFlattenProxy& proxyModel_;

};


//------------------------------------------------------------------------------
WGFlattenProxy::WGFlattenProxy()
	: WGTInterfaceProvider(this)
	, impl_(new Impl(*this))
{
	WGTInterfaceProvider::registerInterface(*impl_);
}


//------------------------------------------------------------------------------
WGFlattenProxy::~WGFlattenProxy()
{
}

void WGFlattenProxy::setSourceModel(QAbstractItemModel* model)
{
	auto source = sourceModel();
	if (model == source)
	{
		return;
	}

	beginResetModel();

	impl_->reset();
	impl_->connections_.reset();

	QAbstractProxyModel::setSourceModel(model);

	if (model != nullptr)
	{
		impl_->flatten(*model);
	}

	if (model != nullptr)
	{
		impl_->connections_ += QObject::connect(model, &QAbstractItemModel::modelAboutToBeReset, [this]() {
			beginResetModel();
			impl_->reset();
		});
		impl_->connections_ +=
			QObject::connect(model, &QAbstractItemModel::modelReset, [this]() {
			auto source = sourceModel();
			impl_->flatten(*source);
			endResetModel(); 
		});

		impl_->connections_ +=
			QObject::connect(model, &QAbstractItemModel::dataChanged,
				[this](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles) {
			auto parent = topLeft.parent();
			if (parent != bottomRight.parent())
			{
				return;
			}

			auto proxyTopLeft = mapFromSource(topLeft);
			auto proxyBottomRight = mapFromSource(bottomRight);
			dataChanged(proxyTopLeft, proxyBottomRight, roles);
		});

		impl_->connections_ += QObject::connect(model, &QAbstractItemModel::rowsInserted, this, &WGFlattenProxy::onSourceRowsInserted);
		impl_->connections_ += QObject::connect(model, &QAbstractItemModel::rowsRemoved, this, &WGFlattenProxy::onSourceRowsRemoved);

		impl_->connections_ += QObject::connect(model, &QAbstractItemModel::columnsAboutToBeInserted,
			[this](const QModelIndex& parent, int first, int last) {
			beginInsertColumns(QModelIndex(), first, last);
		});
		impl_->connections_ += QObject::connect(model, &QAbstractItemModel::columnsInserted,
			[this](const QModelIndex& parent, int first, int last) {
			endInsertColumns();
		});
		impl_->connections_ += QObject::connect(model, &QAbstractItemModel::columnsAboutToBeRemoved,
			[this](const QModelIndex& parent, int first, int last) {
			beginRemoveColumns(QModelIndex(), first, last);
		});
		impl_->connections_ += QObject::connect(model, &QAbstractItemModel::columnsRemoved,
			[this](const QModelIndex& parent, int first, int last) {
			endRemoveColumns();
		});
	}

	endResetModel();
}

QModelIndex WGFlattenProxy::mapToSource(const QModelIndex& proxyIndex) const
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return QModelIndex();
	}

	return impl_->proxy_to_source(proxyIndex, *source);
}

QModelIndex WGFlattenProxy::mapFromSource(const QModelIndex& sourceIndex) const
{
	return impl_->source_to_proxy(sourceIndex, *this);
}

QModelIndex WGFlattenProxy::index(int row, int column, const QModelIndex& parent) const
{
	auto source = sourceModel();
	if (source == nullptr || parent.isValid())
	{
		return QModelIndex();
	}	

	return createIndex(row, column, nullptr);
}

QModelIndex WGFlattenProxy::parent(const QModelIndex& child) const
{
	return QModelIndex();
}

int WGFlattenProxy::rowCount(const QModelIndex& parent) const
{
	auto source = sourceModel();
	if (source == nullptr || parent.isValid())
	{
		return 0;
	}

	return impl_->mapping_.size_;
}

int WGFlattenProxy::columnCount(const QModelIndex& parent) const
{
	auto source = sourceModel();
	if (source == nullptr || parent.isValid())
	{
		return 0;
	}

	return source->columnCount();
}

bool WGFlattenProxy::hasChildren(const QModelIndex& parent) const
{
	return false;
}

QVariant WGFlattenProxy::headerData(int section, Qt::Orientation orientation, int role) const
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return QVariant();
	}

	return source->headerData(section, orientation, role);
}

bool WGFlattenProxy::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return false;
	}

	return source->setHeaderData(section, orientation, value, role);
}

QHash<int, QByteArray> WGFlattenProxy::roleNames() const
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return QHash<int, QByteArray>();
	}

	return source->roleNames();
}

void WGFlattenProxy::onSourceRowsInserted(const QModelIndex& parent, int first, int last)
{
	auto source = sourceModel();
	TF_ASSERT(source != nullptr);
	std::vector<std::unique_ptr<Impl::Mapping>> newChildren;
	impl_->flatten(parent, first, last, *source, newChildren);
	TF_ASSERT(newChildren.size() == last - first + 1);

	auto mapping = impl_->get_mapping(parent);
	auto proxyFirst = mapping->getIndex() + 1;
	for (int i = 0; i < first; ++i)
	{
		proxyFirst += mapping->children_[i]->size_ + 1;
	}
	auto proxyLast = proxyFirst - 1;
	for (auto& child : newChildren)
	{
		child->parent_ = mapping;
		proxyLast += child->size_ + 1;
	}

	beginInsertRows(QModelIndex(), proxyFirst, proxyLast);

	impl_->grow(mapping, proxyLast - proxyFirst + 1);
	auto it = mapping->children_.begin();
	std::advance(it, first);
	mapping->children_.insert(it, make_move_iterator(newChildren.begin()), make_move_iterator(newChildren.end()));

	endInsertRows();
}

void WGFlattenProxy::onSourceRowsRemoved(const QModelIndex& parent, int first, int last)
{
	auto count = last - first + 1;

	auto mapping = impl_->get_mapping(parent);
	auto proxyFirst = mapping->getIndex() + 1;
	for (int i = 0; i < first; ++i)
	{
		proxyFirst += mapping->children_[i]->size_ + 1;
	}
	auto proxyLast = proxyFirst - 1;
	for (int i = first; i < last + 1; ++i)
	{
		proxyLast += mapping->children_[i]->size_ + 1;
	}
	auto proxyCount = proxyLast - proxyFirst + 1;

	beginRemoveRows(QModelIndex(), proxyFirst, proxyLast);

	impl_->grow(mapping, -proxyCount);
	mapping->children_.erase(mapping->children_.begin() + first, mapping->children_.begin() + last + 1);

	endRemoveRows();
}

}