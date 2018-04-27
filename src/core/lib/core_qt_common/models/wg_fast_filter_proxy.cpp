#include "wg_fast_filter_proxy.hpp"

#include "core_common/assert.hpp"
#include "core_common/scoped_stop_watch.hpp"
#include "core_qt_common/interfaces/i_wgt_interface_provider.hpp"
#include "core_qt_common/interfaces/i_wgt_item_model.hpp"
#include "core_qt_common/models/wgt_item_model_base.hpp"
#include "core_qt_common/filter_expression.hpp"
#include "wg_types/shared_string.hpp"
#include "core_variant/variant.hpp"

namespace wgt
{

struct WGFastFilterProxy::Impl
{
	Impl() : compare_(*this), filterRole_("display")
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

		auto proxyRow = proxyIndex.row();
		if (proxyRow >= static_cast<int>(mappedItems_.size()))
		{
			return QModelIndex();
		}

		auto sourceRow = mappedItems_[proxyRow];
		return sourceModel.index(sourceRow, proxyIndex.column());
	}

	QModelIndex source_to_proxy(const QModelIndex& sourceIndex, const WGFastFilterProxy& proxyModel)
	{
		if (!sourceIndex.isValid())
		{
			return QModelIndex();
		}

		auto sourceRow = sourceIndex.row();
		auto it = std::find(mappedItems_.begin(), mappedItems_.end(), sourceRow);
		if (it == mappedItems_.end())
		{
			return QModelIndex();
		}

		auto proxyRow = std::distance(mappedItems_.begin(), it);
		return proxyModel.index(proxyRow, sourceIndex.column());
	}

	void reset()
	{
		sourceValues_.clear();
		sortedSourceValueIndices_.clear();
		mappedItems_.clear();
	}

	void cacheSourceValues(const QAbstractItemModel& sourceModel)
	{
		sourceValues_.clear();
		sortedSourceValueIndices_.clear();

		filterRoleId_ = -1;
		auto roleNames = sourceModel.roleNames();
		for (auto it = roleNames.begin(); it != roleNames.end(); ++it)
		{
			if (it.value() == filterRole_.c_str())
			{
				filterRoleId_ = it.key();
			}
		}
		if (filterRoleId_ == -1)
		{
			return;
		}

		auto rowCount = sourceModel.rowCount();
		sourceValues_.reserve(rowCount);

		auto interfaceProvider = 
			IWGTInterfaceProvider::getInterfaceProvider( sourceModel );
		auto wgtItemModel =
			interfaceProvider ? interfaceProvider->queryInterface< IWgtItemModel >() : nullptr;
		QueryHelper queryHelper;
		wgtItemModel = wgtItemModel && wgtItemModel->canUse(queryHelper) ? wgtItemModel : nullptr;

		SharedString sourceValue;
		for (auto row = 0; row < rowCount; ++row)
		{
			auto sourceIndex = sourceModel.index(row, 0);
			bool ok = false;
			if (wgtItemModel)
			{
				queryHelper.reset();
				auto sourceData = 
					wgtItemModel->variantData( 
						queryHelper, sourceIndex, filterRoleId_);
				ok = sourceData.tryCast(sourceValue);
			}

			if (ok == false)
			{
				auto sourceData = sourceModel.data(sourceIndex, filterRoleId_);
				sourceValue = sourceData.toString().toUtf8().data();
			}
			sourceValues_.push_back(sourceValue);
		}

		sortedSourceValueIndices_.reserve(rowCount);
		for (auto i = 0; i < rowCount; ++i)
		{
			sortedSourceValueIndices_.push_back(i);
		}
		std::sort(sortedSourceValueIndices_.begin(), sortedSourceValueIndices_.end(), compare_);
	}

	void map()
	{
		mappedItems_.clear();

		size_t rowCount = sourceValues_.size();
		mappedItems_.reserve(rowCount);

		for (size_t row = 0; row < rowCount; ++row)
		{
			auto && sourceValue = sourceValues_[row];
			if (filterExpression_.match(sourceValue.str()))
			{
				mappedItems_.push_back(static_cast<int>(row));
			}
		}
	}

	class SourceValueCompare
	{
	public:
		SourceValueCompare(Impl& impl) : impl_(impl) {}

		bool operator()(const size_t &a, const size_t &b)
		{
			return _stricmp(impl_.sourceValues_[a]->c_str(), impl_.sourceValues_[b]->c_str()) < 0;
		}

		bool operator()(const size_t &a, const std::string &value)
		{
			return _stricmp( impl_.sourceValues_[a]->c_str(), value.c_str() ) < 0;
		}

	private:
		Impl& impl_;
	};

	QtConnectionHolder connections_;
	SourceValueCompare compare_;
	std::vector<SharedString> sourceValues_;
	std::vector<size_t> sortedSourceValueIndices_;
	std::vector<int> mappedItems_;
	std::string filterRole_;
	std::string filterText_;
	FilterExpression filterExpression_;
	int filterRoleId_;
};

WGFastFilterProxy::WGFastFilterProxy() : impl_(new Impl())
{
}

WGFastFilterProxy::~WGFastFilterProxy()
{
}

void WGFastFilterProxy::setSourceModel(QAbstractItemModel* model)
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
		impl_->cacheSourceValues(*model);
		impl_->map();
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
			impl_->cacheSourceValues(*source);
			impl_->map();
			endResetModel(); 
		});

		impl_->connections_ += QObject::connect(model, &QAbstractItemModel::dataChanged, this, &WGFastFilterProxy::onSourceDataChanged);
		impl_->connections_ += QObject::connect(model, &QAbstractItemModel::rowsInserted, this, &WGFastFilterProxy::onSourceRowsInserted);
		impl_->connections_ += QObject::connect(model, &QAbstractItemModel::rowsRemoved, this, &WGFastFilterProxy::onSourceRowsRemoved);

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

QModelIndex WGFastFilterProxy::mapToSource(const QModelIndex& proxyIndex) const
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return QModelIndex();
	}

	return impl_->proxy_to_source(proxyIndex, *source);
}

QModelIndex WGFastFilterProxy::mapFromSource(const QModelIndex& sourceIndex) const
{
	return impl_->source_to_proxy(sourceIndex, *this);
}

QModelIndex WGFastFilterProxy::index(int row, int column, const QModelIndex& parent) const
{
	auto source = sourceModel();
	if (source == nullptr || parent.isValid())
	{
		return QModelIndex();
	}	

	return createIndex(row, column, nullptr);
}

QModelIndex WGFastFilterProxy::parent(const QModelIndex& child) const
{
	return QModelIndex();
}

int WGFastFilterProxy::rowCount(const QModelIndex& parent) const
{
	return static_cast<int>(impl_->mappedItems_.size());
}

int WGFastFilterProxy::columnCount(const QModelIndex& parent) const
{
	auto source = sourceModel();
	if (source == nullptr || parent.isValid())
	{
		return 0;
	}

	return source->columnCount();
}

bool WGFastFilterProxy::hasChildren(const QModelIndex& parent) const
{
	return false;
}

QVariant WGFastFilterProxy::headerData(int section, Qt::Orientation orientation, int role) const
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return QVariant();
	}

	return source->headerData(section, orientation, role);
}

bool WGFastFilterProxy::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return false;
	}

	return source->setHeaderData(section, orientation, value, role);
}

QHash<int, QByteArray> WGFastFilterProxy::roleNames() const
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return QHash<int, QByteArray>();
	}

	return source->roleNames();
}

QString WGFastFilterProxy::tryComplete(const QString& str, bool multi) const
{
	std::string data = str.toUtf8().data();
	auto pos = multi ? data.find_last_of(" +!") : -1;
	auto testStr = data.substr(pos + 1);
	if (testStr.empty())
	{
		return str;
	}

	auto it = std::lower_bound(impl_->sortedSourceValueIndices_.begin(), impl_->sortedSourceValueIndices_.end(), testStr, impl_->compare_);
	if (it != impl_->sortedSourceValueIndices_.end())
	{
		auto& sourceValue = impl_->sourceValues_[*it];
		if (_strnicmp(sourceValue->c_str(), testStr.c_str(), testStr.size() ) == 0)
		{
			return (data.substr(0, pos + 1) + sourceValue.str()).c_str();
		}
	}

	return str;
}

QString WGFastFilterProxy::getFilterRole() const
{
	return impl_->filterRole_.c_str();
}

int WGFastFilterProxy::getFilterRoleId() const
{
	return impl_->filterRoleId_;
}

QString WGFastFilterProxy::getFilterText() const
{
	return QString::fromStdString(impl_->filterExpression_.getExpression());
}

void WGFastFilterProxy::setFilterRole(const QString& filterRole)
{
	std::string data = filterRole.toUtf8().data();
	if (impl_->filterRole_ == data)
	{
		return;
	}

	impl_->filterRole_ = data;
	emit filterRoleChanged();

	auto source = sourceModel();
	if (source == nullptr)
	{
		return;
	}

	impl_->cacheSourceValues(*source);
	onFilterChanged();
}

void WGFastFilterProxy::setFilterText(const QString& filterText)
{
	std::string filterTextUtf8 = filterText.toUtf8();
	if (impl_->filterExpression_.getExpression() == filterTextUtf8)
	{
		return;
	}

	impl_->filterExpression_.reset(filterTextUtf8);

	emit filterTextChanged();
	onFilterChanged();
}

void WGFastFilterProxy::onFilterChanged()
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return;
	}

	auto preRowCount = static_cast<int>(impl_->mappedItems_.size());
	impl_->map();
	auto postRowCount = static_cast<int>(impl_->mappedItems_.size());

	if (preRowCount > postRowCount)
	{
		beginRemoveRows(QModelIndex(), postRowCount, preRowCount - 1);
		endRemoveRows();
	}
	else if (preRowCount < postRowCount)
	{
		beginInsertRows(QModelIndex(), preRowCount, postRowCount - 1);
		endInsertRows();
	}

	if (postRowCount == 0)
	{
		return;
	}

	auto proxyTopLeft = index(0, 0);
	auto proxyBottomRight = index(postRowCount - 1, columnCount() - 1);
	dataChanged(proxyTopLeft, proxyBottomRight);
}

void WGFastFilterProxy::onSourceDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
	auto source = sourceModel();
	TF_ASSERT(source != nullptr);

	if (topLeft.parent().isValid() || bottomRight.parent().isValid())
	{
		return;
	}

	if (!roles.isEmpty() && !roles.contains(impl_->filterRoleId_))
	{
		return;
	}

	auto sourceFirst = topLeft.row();
	auto sourceLast = bottomRight.row();

	auto sourceBegin = impl_->sourceValues_.begin();
	std::advance(sourceBegin, sourceFirst);
	auto sourceEnd = impl_->sourceValues_.begin();
	std::advance(sourceEnd, sourceLast + 1);

	auto proxyBegin = std::lower_bound(impl_->mappedItems_.begin(), impl_->mappedItems_.end(), sourceFirst);
	auto proxyEnd = std::upper_bound(impl_->mappedItems_.begin(), impl_->mappedItems_.end(), sourceLast);

	auto proxyFirst = std::distance(impl_->mappedItems_.begin(), proxyBegin);
	auto proxyLast = std::distance(impl_->mappedItems_.begin(), proxyEnd) - 1;

	auto sourceCount = sourceLast - sourceFirst + 1;
	std::vector<int> mappedItems;
	mappedItems.reserve(sourceCount);
	auto sourceRow = sourceFirst;
	for (auto it = sourceBegin; it != sourceEnd; ++it, ++sourceRow)
	{
		auto sourceIndex = source->index(sourceRow, 0);
		auto sourceData = source->data(sourceIndex, impl_->filterRoleId_);
		*it = sourceData.toString().toUtf8().data();
		if (impl_->filterExpression_.match(it->str()))
		{
			mappedItems.push_back(sourceRow);
		}
	}
	std::sort(impl_->sortedSourceValueIndices_.begin(), impl_->sortedSourceValueIndices_.end(), impl_->compare_);

	auto preProxyCount = static_cast<int>(std::distance(proxyBegin, proxyEnd));
	auto postProxyCount = static_cast<int>(mappedItems.size());

	if (preProxyCount > postProxyCount)
	{
		auto removeCount = preProxyCount - postProxyCount;
		beginRemoveRows(QModelIndex(), proxyFirst, proxyFirst + removeCount - 1);
		endRemoveRows();
		proxyLast -= removeCount;
	}
	else if (preProxyCount < postProxyCount)
	{
		auto insertCount = postProxyCount - preProxyCount;
		beginInsertRows(QModelIndex(), proxyFirst, proxyFirst + insertCount - 1);
		endInsertRows();
		proxyLast += insertCount;
	}

	TF_ASSERT(proxyFirst <= proxyLast);
	auto proxyTopLeft = index(proxyFirst, 0);
	auto proxyBottomRight = index(proxyLast, columnCount() - 1);
	dataChanged(proxyTopLeft, proxyBottomRight);
}

void WGFastFilterProxy::onSourceRowsInserted(const QModelIndex& parent, int first, int last)
{
	auto source = sourceModel();
	TF_ASSERT(source != nullptr);

	if (impl_->filterRoleId_ == -1)
	{
		return;
	}
	
	std::vector<std::string> sourceValues;
	sourceValues.reserve(last - first + 1);
	for (auto sourceRow = first; sourceRow <= last; ++sourceRow)
	{
		auto sourceIndex = source->index(sourceRow, 0);
		auto sourceData = source->data(sourceIndex, impl_->filterRoleId_);
		std::string sourceValue = sourceData.toString().toUtf8().data();
		std::transform(sourceValue.begin(), sourceValue.end(), sourceValue.begin(), ::tolower);
		sourceValues.push_back(sourceValue);
	}

	auto sourceCount = static_cast<int>(sourceValues.size());
	std::vector<int> mappedItems;
	mappedItems.reserve(sourceCount);
	for (auto i = 0; i < sourceCount; ++i)
	{
		auto sourceValue = sourceValues[i];
		if (impl_->filterExpression_.match(sourceValue))
		{
			mappedItems.push_back(first + i);
		}
	}

	auto proxyBegin = std::lower_bound(impl_->mappedItems_.begin(), impl_->mappedItems_.end(), first);

	auto proxyCount = static_cast<int>(mappedItems.size());
	if (proxyCount > 0)
	{
		auto proxyFirst = std::distance(impl_->mappedItems_.begin(), proxyBegin);
		auto proxyLast = proxyFirst + proxyCount - 1;
		beginInsertRows(QModelIndex(), proxyFirst, proxyLast);
	}

	auto sourceBegin = impl_->sourceValues_.begin();
	std::advance(sourceBegin, first);
	impl_->sourceValues_.insert(sourceBegin, sourceValues.begin(), sourceValues.end());
	for (auto& sortedSourceValueIndex : impl_->sortedSourceValueIndices_)
	{
		if (static_cast<int>(sortedSourceValueIndex) >= first)
		{
			sortedSourceValueIndex += sourceCount;
		}
	}
	for (auto i = 0; i < sourceCount; ++i)
	{
		auto it = std::lower_bound(impl_->sortedSourceValueIndices_.begin(), impl_->sortedSourceValueIndices_.end(), first + i, impl_->compare_);
		impl_->sortedSourceValueIndices_.insert(it, first + i);
	}

	for (auto it = proxyBegin; it != impl_->mappedItems_.end(); ++it)
	{
		*it += sourceCount;
	}
	impl_->mappedItems_.insert(proxyBegin, mappedItems.begin(), mappedItems.end());

	if (proxyCount > 0)
	{
		endInsertRows();
	}
}

void WGFastFilterProxy::onSourceRowsRemoved(const QModelIndex& parent, int first, int last)
{
	if (impl_->sourceValues_.empty() && impl_->sortedSourceValueIndices_.empty())
	{
		return;
	}

	TF_ASSERT(first <= last);
	auto sourceCount = last - first + 1;

	auto proxyFirst = -1;
	auto proxyLast = -1;
	auto proxyShift = -1;
	auto proxyRowCount = impl_->mappedItems_.size();
	for (size_t proxyRow = 0; proxyRow < proxyRowCount; ++proxyRow)
	{
		auto sourceRow = impl_->mappedItems_[proxyRow];
		if (sourceRow < first)
		{
			continue;
		}

		if (sourceRow > last)
		{
			proxyShift = static_cast<int>(proxyRow);
			break;
		}

		if (proxyFirst == -1)
		{
			proxyFirst = static_cast<int>(proxyRow);
		}
		proxyLast = static_cast<int>(proxyRow);
	}
	TF_ASSERT(proxyFirst <= proxyLast);

	auto sourceEnd = impl_->sourceValues_.begin();
	std::advance(sourceEnd, last + 1);
	auto sourceBegin = impl_->sourceValues_.begin();
	TF_ASSERT(std::distance(sourceBegin, sourceEnd) <= static_cast<ptrdiff_t>(impl_->sourceValues_.size()));
	std::advance(sourceBegin, first);
	impl_->sourceValues_.erase(sourceBegin, sourceEnd);
	for (auto i = 0; i < sourceCount; ++i)
	{
		auto it = std::find(impl_->sortedSourceValueIndices_.begin(), impl_->sortedSourceValueIndices_.end(), first + i);
		TF_ASSERT(it != impl_->sortedSourceValueIndices_.end());
		impl_->sortedSourceValueIndices_.erase(it);
	}
	for (auto& sortedSourceValueIndex : impl_->sortedSourceValueIndices_)
	{
		if (static_cast<int>(sortedSourceValueIndex) >= first)
		{
			sortedSourceValueIndex -= sourceCount;
		}
	}

	if (proxyShift != -1)
	{
		auto it = impl_->mappedItems_.begin();
		std::advance(it, proxyShift);
		for (; it != impl_->mappedItems_.end(); ++it)
		{
			*it -= sourceCount;
		}
	}

	if (proxyFirst == -1)
	{
		return;
	}

	beginRemoveRows(QModelIndex(), proxyFirst, proxyLast);

	auto proxyBegin = impl_->mappedItems_.begin();
	std::advance(proxyBegin, proxyFirst);
	auto proxyEnd = impl_->mappedItems_.begin();
	std::advance(proxyEnd, proxyLast + 1);
	impl_->mappedItems_.erase(proxyBegin, proxyEnd);

	endRemoveRows();
}

}