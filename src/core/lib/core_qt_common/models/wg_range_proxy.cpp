#include "wg_range_proxy.hpp"

#include "core_common/assert.hpp"

#include <deque>

namespace wgt
{
struct WGRangeProxy::Impl
{
	Impl()
	{
	}

	QModelIndex proxy_to_source(const QModelIndex& proxyIndex, QAbstractItemModel& sourceModel) const
	{
		if (!proxyIndex.isValid())
		{
			return QModelIndex();
		}

		return sourceModel.index(range_.top_ + proxyIndex.row(), range_.left_ + proxyIndex.column());
	}

	enum IndexBound
	{
		None = 0,
		Lower = 1,
		Upper = 2
	};
	QModelIndex source_to_proxy(const QModelIndex& sourceIndex, const WGRangeProxy& proxyModel,
	                            IndexBound indexBound = IndexBound::None)
	{
		if (!sourceIndex.isValid())
		{
			return QModelIndex();
		}

		auto source_parent = sourceIndex.parent();
		if (source_parent.isValid())
		{
			return QModelIndex();
		}

		auto source_row = sourceIndex.row();
		if (source_row < range_.top_)
		{
			source_row = indexBound == IndexBound::Lower ? range_.top_ : -1;
		}
		else if (source_row > range_.bottom_)
		{
			source_row = indexBound == IndexBound::Upper ? range_.bottom_ : -1;
		}

		auto source_column = sourceIndex.column();
		if (source_column < range_.left_)
		{
			source_column = indexBound == IndexBound::Lower ? range_.left_ : -1;
		}
		else if (source_column > range_.right_)
		{
			source_column = indexBound == IndexBound::Upper ? range_.right_ : -1;
		}

		if (source_row < 0 || source_column < 0)
		{
			return QModelIndex();
		}

		return proxyModel.createIndex(source_row - range_.top_, source_column - range_.right_);
	}

	QtConnectionHolder connections_;
	struct Range
	{
		Range(int top = -1, int left = -1, int bottom = -1, int right = -1)
		    : top_(top), left_(left), bottom_(bottom), right_(right)
		{
		}

		int top_;
		int left_;
		int bottom_;
		int right_;
	};
	Range range_;
	std::deque<std::pair<int, int>> pendingRowChanges_;
	std::deque<std::pair<int, int>> pendingColumnChanges_;

	class RowOperator
	{
	public:
		RowOperator(WGRangeProxy& proxy) : proxy_(proxy)
		{
		}

		int& front()
		{
			return proxy_.impl_->range_.top_;
		}
		int& back()
		{
			return proxy_.impl_->range_.bottom_;
		}

		int sourceCount() const
		{
			return proxy_.sourceModel()->rowCount();
		}

		void beginRemove(int first, int last)
		{
			proxy_.beginRemoveRows(QModelIndex(), first, last);
		}
		void endRemove()
		{
			proxy_.endRemoveRows();
		}

		void beginInsert(int first, int last)
		{
			proxy_.beginInsertRows(QModelIndex(), first, last);
		}
		void endInsert()
		{
			proxy_.endInsertRows();
		}

		void beginMove(int start, int end, int dest)
		{
			proxy_.beginMoveRows(QModelIndex(), start, end, QModelIndex(), dest);
		}
		void endMove()
		{
			proxy_.endMoveRows();
		}

		void dataChanged(int first, int last)
		{
			proxy_.rowsChanged(first, last);
		}
		void pushDataChange(int first, int last)
		{
			proxy_.impl_->pendingRowChanges_.emplace_back(std::make_pair(first, last));
		}

	private:
		WGRangeProxy& proxy_;
	};

	class ColumnOperator
	{
	public:
		ColumnOperator(WGRangeProxy& proxy) : proxy_(proxy)
		{
		}

		int& front()
		{
			return proxy_.impl_->range_.left_;
		}
		int& back()
		{
			return proxy_.impl_->range_.right_;
		}

		int sourceCount() const
		{
			return proxy_.sourceModel()->columnCount();
		}

		void beginRemove(int first, int last)
		{
			proxy_.beginRemoveColumns(QModelIndex(), first, last);
		}
		void endRemove()
		{
			proxy_.endRemoveColumns();
		}

		void beginInsert(int first, int last)
		{
			proxy_.beginInsertColumns(QModelIndex(), first, last);
		}
		void endInsert()
		{
			proxy_.endInsertColumns();
		}

		void beginMove(int start, int end, int dest)
		{
			proxy_.beginMoveColumns(QModelIndex(), start, end, QModelIndex(), dest);
		}
		void endMove()
		{
			proxy_.endMoveColumns();
		}

		void dataChanged(int first, int last)
		{
			proxy_.columnsChanged(first, last);
		}
		void pushDataChange(int first, int last)
		{
			proxy_.impl_->pendingColumnChanges_.emplace_back(std::make_pair(first, last));
		}

	private:
		WGRangeProxy& proxy_;
	};

	template <class T>
	void setRange(WGRangeProxy& proxy, int first, int last)
	{
		T tOperator(proxy);

		auto& front = tOperator.front();
		auto& back = tOperator.back();

		if ((first < front && last < front) || (first > back && last > back))
		{
			auto removeCount = back - front + 1;
			auto insertCount = last - first + 1;

			if (removeCount > insertCount)
			{
				tOperator.beginRemove(0, removeCount - insertCount - 1);
				front = first;
				back = last;
				tOperator.endRemove();

				tOperator.dataChanged(0, insertCount - 1);
			}
			else if (insertCount > removeCount)
			{
				tOperator.beginInsert(0, insertCount - removeCount - 1);
				front = first;
				back = last;
				tOperator.endInsert();

				tOperator.dataChanged(insertCount - removeCount, insertCount - 1);
			}
			else
			{
				front = first;
				back = last;

				tOperator.dataChanged(0, removeCount - 1);
			}
		}
		else
		{
			auto frontChange = front - first;
			auto backChange = last - back;

			auto frontBackChange = 0;
			if (frontChange > 0 && backChange < 0)
			{
				frontBackChange = std::max(-frontChange, backChange);
			}
			else if (frontChange < 0 && backChange > 0)
			{
				frontBackChange = std::min(-frontChange, backChange);
			}
			frontChange += frontBackChange;
			backChange -= frontBackChange;

			if (frontChange > 0)
			{
				// insert items at front
				tOperator.beginInsert(0, frontChange - 1);
				front -= frontChange;
				tOperator.endInsert();
			}
			else if (frontChange < 0)
			{
				// remove items from front
				tOperator.beginRemove(0, -frontChange - 1);
				front -= frontChange;
				tOperator.endRemove();
			}

			if (backChange > 0)
			{
				// insert items at back
				tOperator.beginInsert(back - front + 1, back - front + backChange);
				back += backChange;
				tOperator.endInsert();
			}
			else if (backChange < 0)
			{
				// remove items at back
				tOperator.beginRemove(back - front + 1 + backChange, back - front);
				back += backChange;
				tOperator.endRemove();
			}

			if (frontBackChange > 0)
			{
				// move items from front to back
				tOperator.beginMove(0, frontBackChange - 1, back - front + 1);
				front += frontBackChange;
				back += frontBackChange;
				tOperator.endMove();

				tOperator.dataChanged(back - front - frontBackChange + 1, back - front);
			}
			else if (frontBackChange < 0)
			{
				// move items from back to front
				tOperator.beginMove(back - front + frontBackChange + 1, back - front, 0);
				front += frontBackChange;
				back += frontBackChange;
				tOperator.endMove();

				tOperator.dataChanged(0, -frontBackChange - 1);
			}
		}
	}

	template <class T>
	void sourceAboutToBeInserted(WGRangeProxy& proxy, int first, int last)
	{
		T tOperator(proxy);

		auto& front = tOperator.front();
		auto& back = tOperator.back();

		if (first > back)
		{
			return;
		}

		if (first <= front)
		{
			auto shiftToBack = last - first + 1;
			front += shiftToBack;
			back += shiftToBack;
			return;
		}

		if (last < back)
		{
			// move the rows that will be pushed off the end forward so we can replace them
			tOperator.beginMove(back - front - (last - first), back - front, first - front);
			tOperator.endMove();
		}

		tOperator.pushDataChange(first - front, std::min(last, back) - front);
	}

	template <class T>
	void sourceAboutToBeRemoved(WGRangeProxy& proxy, int first, int last)
	{
		T tOperator(proxy);

		auto available = tOperator.sourceCount();
		auto& front = tOperator.front();
		auto& back = tOperator.back();

		if (first > back)
		{
			return;
		}

		if (first < front)
		{
			// delete items before the visible range
			auto shiftToFront = std::min(last, front - 1) - first + 1;
			available -= shiftToFront;
			front -= shiftToFront;
			back -= shiftToFront;
			last -= shiftToFront;

			if (last < front)
			{
				return;
			}
			TF_ASSERT(first == front);
		}

		if (last < back)
		{
			tOperator.beginMove(last - front + 1, back - front, first - front);
			first += back - last;
			last += back - last;
			tOperator.endMove();
		}

		auto removeCount = back - first + 1;
		auto availableBack = available - (last + 1);
		if (availableBack < removeCount)
		{
			auto shiftToFront = std::min(removeCount - availableBack, front);
			if (shiftToFront > 0)
			{
				tOperator.beginMove(back - front - shiftToFront + 1, back - front, 0);
				front -= shiftToFront;
				back -= shiftToFront;
				tOperator.endMove();

				tOperator.pushDataChange(0, shiftToFront - 1);
			}
		}

		// try again
		removeCount = back - first + 1;
		if (availableBack < removeCount)
		{
			auto removeBack = removeCount - availableBack;
			tOperator.beginRemove(back - front - removeBack + 1, back - front);
			back -= removeBack;
			tOperator.endRemove();
		}

		removeCount = back - first + 1;
		TF_ASSERT(availableBack >= removeCount);

		if (removeCount > 0)
		{
			tOperator.pushDataChange(back - front - removeCount + 1, back - front);
		}
	}

	template <class T>
	void sourceAboutToBeMoved(WGRangeProxy& proxy, int start, int end, int dest)
	{
		TF_ASSERT(dest < start || dest > end + 1);

		T tOperator(proxy);

		auto& front = tOperator.front();
		auto& back = tOperator.back();

		if (start > back)
		{
			sourceAboutToBeInserted<T>(proxy, dest, dest + start - end);
			return;
		}
		else if (end < front)
		{
			if (dest <= front)
			{
				return;
			}

			auto shiftToFront = start - end + 1;
			front -= shiftToFront;
			back -= shiftToFront;
			dest -= shiftToFront;

			if (dest > back)
			{
				return;
			}

			sourceAboutToBeInserted<T>(proxy, dest, dest + start - end);
			return;
		}

		if (dest > back)
		{
			sourceAboutToBeRemoved<T>(proxy, start, end);
			return;
		}
		else if (dest <= front)
		{
			if (end < front)
			{
				return;
			}

			auto shiftToBack = start - end + 1;
			front += shiftToBack;
			back += shiftToBack;
			start += shiftToBack;
			end += shiftToBack;

			if (start > back)
			{
				return;
			}

			sourceAboutToBeRemoved<T>(proxy, start, end);
			return;
		}

		if (start >= front && end <= back && dest > front && dest <= back)
		{
			tOperator.beginMove(start - front, end - front, dest - front);
			tOperator.endMove();
			return;
		}

		if (start < front)
		{
			auto frontCopy = front;
			auto backCopy = back;

			auto insertCount = frontCopy - start;
			auto available = backCopy - dest + 1;

			auto changeStart = dest - frontCopy;
			if (insertCount < available)
			{
				auto moveCount = insertCount;
				tOperator.beginMove(backCopy - frontCopy - moveCount + 1, backCopy - frontCopy, dest - frontCopy);
				frontCopy -= moveCount;
				backCopy -= moveCount;
				end -= moveCount;
				tOperator.endMove();
			}
			else
			{
				frontCopy -= available;
				backCopy -= available;
				end -= available;
				insertCount -= available;

				if (insertCount > 0)
				{
					auto moveCount = std::min(end - frontCopy + 1, insertCount);
					tOperator.beginMove(end - frontCopy - moveCount + 1, end - frontCopy, dest - frontCopy);
					end -= moveCount;
					changeStart -= moveCount;
					insertCount -= moveCount;
					tOperator.endMove();
				}
			}
			auto changeEnd = dest - frontCopy - 1;

			if (insertCount == 0)
			{
				auto moveCount = end - frontCopy + 1;
				if (moveCount > 0)
				{
					tOperator.beginMove(0, end - frontCopy, dest - frontCopy);
					end -= moveCount;
					changeStart -= moveCount;
					changeEnd -= moveCount;
					tOperator.endMove();
				}
			}

			tOperator.pushDataChange(changeStart, changeEnd);
			return;
		}

		if (end > back)
		{
			{
				auto moveCount = back - start + 1;
				tOperator.beginMove(back - front - moveCount + 1 - front, back - front, dest - front);
				dest += moveCount;
				start += moveCount;
				tOperator.endMove();
			}

			auto insertCount = end - start + 1;
			auto available = back - dest + 1;

			auto changeStart = dest - front;
			if (insertCount < available)
			{
				auto moveCount = insertCount;
				tOperator.beginMove(back - front - moveCount + 1, back - front, dest - front);
				dest += moveCount;
				start += moveCount;
				tOperator.endMove();
			}
			else
			{
				dest += available;
				start += available;
			}
			auto changeEnd = dest - front - 1;

			tOperator.pushDataChange(changeStart, changeEnd);
			return;
		}

		TF_ASSERT(false);
	}

	void flushPendingRowChanges(WGRangeProxy & proxy)
	{
		while (!pendingRowChanges_.empty())
		{
			auto& rows = pendingRowChanges_.front();
			proxy.rowsChanged(rows.first, rows.second);
			pendingRowChanges_.pop_front();
		}
	}

	void flushPendingColumnChanges(WGRangeProxy & proxy)
	{
		while (!pendingColumnChanges_.empty())
		{
			auto& rows = pendingColumnChanges_.front();
			proxy.rowsChanged(rows.first, rows.second);
			pendingColumnChanges_.pop_front();
		}
	}
};

WGRangeProxy::WGRangeProxy() : impl_(new Impl())
{
}

WGRangeProxy::~WGRangeProxy()
{
}

void WGRangeProxy::setSourceModel(QAbstractItemModel* model)
{
	auto source = sourceModel();
	if (model == source)
	{
		return;
	}

	beginResetModel();
	QAbstractProxyModel::setSourceModel(model);
	reset();
	endResetModel();
}

QModelIndex WGRangeProxy::mapToSource(const QModelIndex& proxyIndex) const
{
	if (!isValid())
	{
		return QModelIndex();
	}

	auto source = sourceModel();
	return impl_->proxy_to_source(proxyIndex, *source);
}

QModelIndex WGRangeProxy::mapFromSource(const QModelIndex& sourceIndex) const
{
	if (!isValid())
	{
		return QModelIndex();
	}

	return impl_->source_to_proxy(sourceIndex, *this);
}

QModelIndex WGRangeProxy::index(int row, int column, const QModelIndex& parent) const
{
	if (!isValid())
	{
		return QModelIndex();
	}

	if (parent.isValid())
	{
		return QModelIndex();
	}

	return createIndex(row, column);
}

QModelIndex WGRangeProxy::parent(const QModelIndex& child) const
{
	return QModelIndex();
}

int WGRangeProxy::rowCount(const QModelIndex& parent) const
{
	if (!isValid())
	{
		return 0;
	}

	if (parent.isValid())
	{
		return 0;
	}

	return impl_->range_.bottom_ - impl_->range_.top_ + 1;
}

int WGRangeProxy::columnCount(const QModelIndex& parent) const
{
	if (!isValid())
	{
		return 0;
	}

	if (parent.isValid())
	{
		return 0;
	}

	return impl_->range_.right_ - impl_->range_.left_ + 1;
}

bool WGRangeProxy::hasChildren(const QModelIndex& parent) const
{
	return false;
}

QVariant WGRangeProxy::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (!isValid())
	{
		return QVariant();
	}

	int source_section;
	if (orientation == Qt::Horizontal)
	{
		source_section = section - impl_->range_.left_;
	}
	else
	{
		source_section = section - impl_->range_.top_;
	}

	if (section < 0)
	{
		return QVariant();
	}

	auto source = sourceModel();
	return source->headerData(section, orientation, role);
}

bool WGRangeProxy::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
	if (!isValid())
	{
		return false;
	}

	int source_section;
	if (orientation == Qt::Horizontal)
	{
		source_section = section - impl_->range_.left_;
	}
	else
	{
		source_section = section - impl_->range_.top_;
	}

	if (section < 0)
	{
		return false;
	}

	auto source = sourceModel();
	return source->setHeaderData(section, orientation, value, role);
}

QHash<int, QByteArray> WGRangeProxy::roleNames() const
{
	if (!isValid())
	{
		return QHash<int, QByteArray>();
	}

	auto source = sourceModel();
	return source->roleNames();
}

void WGRangeProxy::setRange(int top, int left, int bottom, int right)
{
	if (impl_->range_.top_ == top && impl_->range_.left_ == left && impl_->range_.bottom_ == bottom &&
	    impl_->range_.right_ == right)
	{
		return;
	}

	if (!isValid() || left < 0 || top < 0 || right < 0 || bottom < 0)
	{
		beginResetModel();
		impl_->range_.top_ = top;
		impl_->range_.left_ = left;
		impl_->range_.bottom_ = bottom;
		impl_->range_.right_ = right;
		reset();
		endResetModel();
		return;
	}

	impl_->flushPendingRowChanges(*this);
	impl_->flushPendingColumnChanges(*this);

	impl_->setRange<Impl::RowOperator>(*this, top, bottom);
	impl_->setRange<Impl::ColumnOperator>(*this, left, right);
}

QModelIndex WGRangeProxy::mapLowerBound(const QModelIndex& sourceIndex) const
{
	if (!isValid())
	{
		return QModelIndex();
	}

	return impl_->source_to_proxy(sourceIndex, *this, Impl::IndexBound::Lower);
}

QModelIndex WGRangeProxy::mapUpperBound(const QModelIndex& sourceIndex) const
{
	if (!isValid())
	{
		return QModelIndex();
	}

	return impl_->source_to_proxy(sourceIndex, *this, Impl::IndexBound::Upper);
}

bool WGRangeProxy::isValid() const
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return false;
	}

	if (impl_->range_.left_ < 0 || impl_->range_.top_ < 0 || impl_->range_.right_ < 0 || impl_->range_.bottom_ < 0)
	{
		return false;
	}

	if (impl_->range_.top_ > impl_->range_.bottom_ || impl_->range_.left_ > impl_->range_.right_)
	{
		return false;
	}

	return true;
}

void WGRangeProxy::reset()
{
	if (!isValid())
	{
		impl_->connections_.reset();
		return;
	}

	auto source = sourceModel();

	impl_->connections_.reset();

	impl_->connections_ +=
	QObject::connect(source, &QAbstractItemModel::modelAboutToBeReset, [this]() { beginResetModel(); });
	impl_->connections_ += QObject::connect(source, &QAbstractItemModel::modelReset, [this]() { endResetModel(); });

	impl_->connections_ +=
	QObject::connect(source, &QAbstractItemModel::dataChanged,
	[this](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles) {
		if (!isValid())
		{
			return;
		}

		auto proxyTopLeft = mapLowerBound(topLeft);
		auto proxyBottomRight = mapUpperBound(bottomRight);
		if (!proxyTopLeft.isValid() || !proxyBottomRight.isValid())
		{
			return;
		}
		dataChanged(proxyTopLeft, proxyBottomRight, roles);
	});
	impl_->connections_ += QObject::connect(
	source, &QAbstractItemModel::headerDataChanged, [this](Qt::Orientation orientation, int first, int last) {
		if (!isValid())
		{
			return;
		}

		int proxyFirst;
		int proxyLast;
		if (orientation == Qt::Horizontal)
		{
			proxyFirst =
			first < impl_->range_.top_ ? 0 : (first > impl_->range_.bottom_ ? -1 : first - impl_->range_.top_);
			proxyLast = last > impl_->range_.bottom_ ? impl_->range_.bottom_ - impl_->range_.top_ :
			                                           (last < impl_->range_.top_ ? -1 : last - impl_->range_.top_);
		}
		else
		{
			proxyFirst =
			first < impl_->range_.left_ ? 0 : (first > impl_->range_.right_ ? -1 : first - impl_->range_.left_);
			proxyLast = last > impl_->range_.right_ ? impl_->range_.right_ - impl_->range_.left_ :
			                                          (last < impl_->range_.left_ ? -1 : last - impl_->range_.left_);
		}
		if (proxyFirst < 0 || proxyLast < 0)
		{
			return;
		}
		headerDataChanged(orientation, proxyFirst, proxyLast);
	});

	impl_->connections_ += QObject::connect(source, &QAbstractItemModel::rowsAboutToBeInserted,
	[this](const QModelIndex& parent, int first, int last) {
		if (!isValid())
		{
			return;
		}

		if (parent.isValid())
		{
			return;
		}

		impl_->sourceAboutToBeInserted<Impl::RowOperator>(*this, first, last);
	});
	impl_->connections_ +=
	QObject::connect(source, &QAbstractItemModel::rowsInserted, [this](const QModelIndex& parent, int first, int last) {
		if (!isValid())
		{
			return;
		}

		if (parent.isValid())
		{
			return;
		}

		impl_->flushPendingRowChanges(*this);
	});
	impl_->connections_ += QObject::connect(source, &QAbstractItemModel::rowsAboutToBeRemoved,
	[this](const QModelIndex& parent, int first, int last) {
		if (!isValid())
		{
			return;
		}

		if (parent.isValid())
		{
			return;
		}

		impl_->sourceAboutToBeRemoved<Impl::RowOperator>(*this, first, last);
	});
	impl_->connections_ +=
	QObject::connect(source, &QAbstractItemModel::rowsRemoved, [this](const QModelIndex& parent, int first, int last) {
		if (!isValid())
		{
			return;
		}

		if (parent.isValid())
		{
			return;
		}

		impl_->flushPendingRowChanges(*this);
	});
	impl_->connections_ += QObject::connect(
	source, &QAbstractItemModel::rowsAboutToBeMoved,
	[this](const QModelIndex& sourceParent, int sourceStart, int sourceEnd, const QModelIndex& destinationParent,
	       int destinationRow) {
		if (!isValid())
		{
			return;
		}

		if (!sourceParent.isValid() && !destinationParent.isValid())
		{
			impl_->sourceAboutToBeMoved<Impl::RowOperator>(*this, sourceStart, sourceEnd, destinationRow);
		}
		else if (!sourceParent.isValid())
		{
			impl_->sourceAboutToBeRemoved<Impl::RowOperator>(*this, sourceStart, sourceEnd);
		}
		else if (!destinationParent.isValid())
		{
			impl_->sourceAboutToBeInserted<Impl::RowOperator>(*this, destinationRow,
			                                                  destinationRow + sourceEnd - sourceStart);
		}
	});
	impl_->connections_ +=
	QObject::connect(source, &QAbstractItemModel::rowsMoved,
		[this](const QModelIndex& parent, int start, int end, const QModelIndex& destintation, int row) {
		if (!isValid())
		{
			return;
		}
		
		if (parent.isValid() && destintation.isValid())
		{
			return;
		}

		impl_->flushPendingRowChanges(*this);
	});

	impl_->connections_ += QObject::connect(
	source, &QAbstractItemModel::columnsAboutToBeInserted, [this](const QModelIndex& parent, int first, int last) {
		if (!isValid())
		{
			return;
		}

		if (parent.isValid())
		{
			return;
		}

		impl_->sourceAboutToBeInserted<Impl::ColumnOperator>(*this, first, last);
	});
	impl_->connections_ += QObject::connect(source, &QAbstractItemModel::columnsInserted,
		[this](const QModelIndex& parent, int first, int last) {
		if (!isValid())
		{
		    return;
		}
		
		if (parent.isValid())
		{
		    return;
		}

		impl_->flushPendingColumnChanges(*this);
	});
	impl_->connections_ += QObject::connect(source, &QAbstractItemModel::columnsAboutToBeRemoved,
	[this](const QModelIndex& parent, int first, int last) {
		if (!isValid())
		{
			return;
		}

		if (parent.isValid())
		{
			return;
		}

		impl_->sourceAboutToBeRemoved<Impl::ColumnOperator>(*this, first, last);
	});
	impl_->connections_ += QObject::connect(source, &QAbstractItemModel::columnsRemoved,
	[this](const QModelIndex& parent, int first, int last) {
		if (!isValid())
		{
			return;
		}

		if (parent.isValid())
		{
			return;
		}

		impl_->flushPendingColumnChanges(*this);
	});
	impl_->connections_ += QObject::connect(
	source, &QAbstractItemModel::columnsAboutToBeMoved,
	[this](const QModelIndex& sourceParent, int sourceStart, int sourceEnd, const QModelIndex& destinationParent,
	       int destinationColumn) {
		if (!isValid())
		{
			return;
		}

		if (!sourceParent.isValid() && !destinationParent.isValid())
		{
			impl_->sourceAboutToBeMoved<Impl::ColumnOperator>(*this, sourceStart, sourceEnd, destinationColumn);
		}
		else if (!sourceParent.isValid())
		{
			impl_->sourceAboutToBeRemoved<Impl::ColumnOperator>(*this, sourceStart, sourceEnd);
		}
		else if (!destinationParent.isValid())
		{
			impl_->sourceAboutToBeInserted<Impl::ColumnOperator>(*this, destinationColumn,
			                                                     destinationColumn + sourceEnd - sourceStart);
		}
	});
	impl_->connections_ += QObject::connect(
	source, &QAbstractItemModel::columnsMoved,
	[this](const QModelIndex& parent, int start, int end, const QModelIndex& destintation, int column) {
		if (!isValid())
		{
			return;
		}

		if (parent.isValid() && destintation.isValid())
		{
			return;
		}

		impl_->flushPendingColumnChanges(*this);
	});
}
}