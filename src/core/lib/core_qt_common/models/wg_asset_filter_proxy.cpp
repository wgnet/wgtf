#include "wg_asset_filter_proxy.hpp"

#include "wg_types/hash_utilities.hpp"
#include "core_logging/logging.hpp"
#include "core_data_model/common_data_roles.hpp"
#include "core_qt_common/models/wgt_item_model_base.hpp"
#include "core_common/assert.hpp"
#include "core_common/scoped_stop_watch.hpp"
#include "wg_types/shared_string.hpp"
#include "core_variant/variant.hpp"
#include "core_dependency_system/depends.hpp"
#include "editor_interaction/i_background_worker.hpp"
#include <mutex>
#include <QDebug>

namespace wgt
{
namespace AssetFilterProxyDetails
{
struct StateChanger;

//==============================================================================
struct State
{
	friend StateChanger;
public:
	State(std::function< void() > beginReset, std::function< void() > endReset)
		: beginReset_(beginReset)
		, endReset_(endReset)
		, showHidden_( false )
	{
	}

	const std::string & getExtension() const { return extension_; }
	const std::vector<uint64_t> & getExtensionHashes() const { return extensionHashes_; }
	bool getShowHidden() const { return showHidden_; }
	int getIsHiddenRoleId() const { return isHiddenRoleId_; }
	QAbstractItemModel * getSourceModel() const { return sourceModel_; }

	void setHiddenRoleId(int isHiddenRoleId) { isHiddenRoleId_ = isHiddenRoleId; }

	StateChanger getStateChanger() const;

protected:
	std::string extension_;
	std::vector<uint64_t> extensionHashes_;
	bool showHidden_;
	int isHiddenRoleId_;
	QAbstractItemModel * sourceModel_ = nullptr;
	std::function< void() > beginReset_;
	std::function< void() > endReset_;
};

//==============================================================================
struct StateChanger
{
public:
	StateChanger(State & state);
	~StateChanger();
	bool setSourceModel(QAbstractItemModel * model);
	bool setExtension(std::string & extension);
	bool setShowHidden(bool showHidden);
	void invalidate();

private:
	bool dirty_;
	State & state_;
};


//------------------------------------------------------------------------------
StateChanger::StateChanger(State & state)
	: state_(state)
	, dirty_(false)
{
}

//------------------------------------------------------------------------------
StateChanger::~StateChanger()
{
	if (dirty_)
	{
		state_.endReset_();
	}
}


//------------------------------------------------------------------------------
bool StateChanger::setSourceModel(QAbstractItemModel * model)
{
	if (state_.sourceModel_ == model)
	{
		return false;
	}

	invalidate();
	state_.sourceModel_ = model;
	return true;
}

//------------------------------------------------------------------------------
bool StateChanger::setExtension(std::string & extension)
{
	std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
	if (extension == state_.extension_)
	{
		return false;
	}

	invalidate();
	state_.extension_ = extension;
	state_.extensionHashes_.clear();
	if (!extension.empty())
	{
		auto str = extension.c_str();
		while (true)
		{
			auto pos = strchr(str, '|');
			if (pos == nullptr)
			{
				state_.extensionHashes_.push_back(HashUtilities::compute(str));
				break;
			}
			else if (pos > str)
			{
				state_.extensionHashes_.push_back(HashUtilities::compute(str, pos - str));
			}
			str = pos + 1;
		}
	}
	return true;
}


//------------------------------------------------------------------------------
bool StateChanger::setShowHidden(bool showHidden)
{
	if (showHidden == state_.showHidden_)
	{
		return false;
	}
	invalidate();
	state_.showHidden_ = showHidden;
	return true;
}


//------------------------------------------------------------------------------
void StateChanger::invalidate()
{
	if (dirty_)
	{
		return;
	}

	state_.beginReset_();
	dirty_ = true;
}


//==============================================================================
StateChanger State::getStateChanger() const
{
	return StateChanger(const_cast<State &>(*this));
}


//==============================================================================
struct Mapping
{
	bool removing_ = false;

	std::map<uint64_t, int> extensions_;
	bool isHidden_ = false;
	Mapping* parent_ = nullptr;

	std::vector<std::unique_ptr<Mapping>> children_;
	std::vector<Mapping*> visibleChildren_;


	static const size_t INVALID_INDEX = (size_t)-1;
	size_t indexInParent_ = INVALID_INDEX;
	size_t visibleIndexInParent_ = INVALID_INDEX;
};


struct NotificationHelper;

//==============================================================================
struct NotificationImpl
{
	NotificationImpl()
	{
	}

	NotificationImpl(
		std::function< void(int, int) > onStart,
		std::function< void() > onEnd)
		: onStart_(onStart)
		, onEnd_(onEnd)
	{
	}

	std::unique_ptr< NotificationHelper > createHelper(int first, int last) const;

	std::function< void(int, int) > onStart_;
	std::function< void() > onEnd_;
};


//==============================================================================
struct NotificationHelper
{
	NotificationHelper(const NotificationImpl & impl)
		: impl_(impl)
	{

	}

	~NotificationHelper()
	{
		if (impl_.onEnd_)
		{
			impl_.onEnd_();
		}
	}

	const NotificationImpl & impl_;
};


//------------------------------------------------------------------------------
std::unique_ptr< NotificationHelper > NotificationImpl::createHelper(int first, int last) const
{
	if (onStart_ == nullptr)
	{
		return nullptr;
	}
	onStart_(first, last);
	return std::make_unique< NotificationHelper >(*this );
}


//==============================================================================
struct ModelMapping
{
	ModelMapping(const ModelMapping & mapping);
	ModelMapping(std::function< void() > beginReset, std::function< void() > endReset);

	Mapping					mapping_;
	State					state_;

	struct WgtQueryHelper
	{
		IWgtItemModel * itemModel_;
		QueryHelper cachedHelper_;
	};


	static bool isHidden(
		const QModelIndex& sourceIndex, QAbstractItemModel& sourceModel, WgtQueryHelper* helper, const State & state);
	uint64_t extensionHash(const QModelIndex& sourceIndex, QAbstractItemModel& sourceModel, WgtQueryHelper* helper);

	const State & getState() const;
	State & getState();

	bool map(QAbstractItemModel& sourceModel, std::function< bool(void) > & shouldAbort);
	static bool map(
		Mapping* mapping, const QModelIndex& sourceIndex,
		QAbstractItemModel& sourceModel, WgtQueryHelper * helper,
		const State & state,
		std::function< bool(void) > & shouldAbort);


	static void map_insert(
		Mapping* mapping, const QModelIndex& sourceIndex, 
		QAbstractItemModel& sourceModel, int first, int last, 
		WgtQueryHelper * helper, const State & state);
	static void map_pre_remove(Mapping* mapping, int first, int last, WgtQueryHelper * helper);
	static void map_post_remove(Mapping* mapping, int first, int last, WgtQueryHelper * helper);
	static bool mapping_is_visible(const Mapping* mapping, const State & state);
	static void update_visible_mappings(
		Mapping* mapping, const State & state, bool recurse = true)
	{
		update_visible_mappings(NotificationImpl(), mapping, state, recurse);
	}
	static void update_visible_mappings(
		const NotificationImpl & notificationImpl,
		Mapping* mapping, const State & state, bool recurse = true );
};


//==============================================================================
ModelMapping::ModelMapping(const ModelMapping & mapping)
	: state_(mapping.state_)
{
}


//------------------------------------------------------------------------------
ModelMapping::ModelMapping(std::function< void() > beginReset, std::function< void() > endReset)
	: state_(beginReset, endReset)
{
}


//------------------------------------------------------------------------------
bool ModelMapping::isHidden(const QModelIndex& sourceIndex, QAbstractItemModel& sourceModel, WgtQueryHelper* helper , const State & state)
{
	bool isHidden = false;
	
	auto hiddenRoleId = state.getIsHiddenRoleId();
	if (hiddenRoleId != -1 && sourceIndex.isValid())
	{
		bool ok = false;
		if (helper && helper->itemModel_)
		{
			helper->cachedHelper_.reset();
			auto sourceData =
				helper->itemModel_->variantData(
					helper->cachedHelper_, sourceIndex, hiddenRoleId);
			ok = sourceData.tryCast(isHidden);
		}
		if (ok == false)
		{
			isHidden = sourceModel.data(sourceIndex, hiddenRoleId).toBool();
		}
	}
	return isHidden;
}

//------------------------------------------------------------------------------
uint64_t ModelMapping::extensionHash(const QModelIndex& sourceIndex, QAbstractItemModel& sourceModel, WgtQueryHelper* helper)
{
	bool ok = false;
	SharedString sourceValue;
	if (helper && helper->itemModel_)
	{
		helper->cachedHelper_.reset();
		auto sourceData =
			helper->itemModel_->variantData(
				helper->cachedHelper_, sourceIndex, Qt::DisplayRole);
		ok = sourceData.tryCast(sourceValue);
	}

	if (ok == false)
	{
		auto sourceData = sourceModel.data(sourceIndex);
		sourceValue = sourceData.toString().toUtf8().data();
	}

	auto && path = sourceValue.str();
	auto pos = path.find_last_of('.');
	if (pos == std::string::npos)
	{
		return 0;
	}

	auto ext = path.c_str() + pos + 1;
	return HashUtilities::computei(ext);
}

//------------------------------------------------------------------------------
const State & ModelMapping::getState() const
{
	return state_;
}


//------------------------------------------------------------------------------
State & ModelMapping::getState()
{
	return state_;
}

//------------------------------------------------------------------------------
bool ModelMapping::map(
	QAbstractItemModel& sourceModel, std::function< bool(void) > & shouldAbort)
{
	SCOPE_TAG
		state_.setHiddenRoleId(-1);
	auto roleNames = sourceModel.roleNames();
	for (auto it = roleNames.begin(); it != roleNames.end(); ++it)
	{
		if (it.value() == "isHidden")
		{
			state_.setHiddenRoleId(it.key());
			break;
		}
	}

	auto interfaceProvider =
		IWGTInterfaceProvider::getInterfaceProvider(sourceModel);
	WgtQueryHelper helper;
	helper.itemModel_ =
		interfaceProvider ? interfaceProvider->queryInterface< IWgtItemModel >() : nullptr;
	helper.itemModel_ = helper.itemModel_&& helper.itemModel_->canUse(helper.cachedHelper_) ? helper.itemModel_ : nullptr;

	if (map(&mapping_, QModelIndex(), sourceModel, &helper, getState(), shouldAbort) == false)
	{
		return false;
	}
	update_visible_mappings(&mapping_, state_);
	return shouldAbort() == false;
}


//------------------------------------------------------------------------------
bool ModelMapping::map(
	Mapping* mapping, const QModelIndex& sourceIndex,
	QAbstractItemModel& sourceModel, WgtQueryHelper * helper,
	const State & state,
	std::function< bool(void) > & shouldAbort)
{
	if (shouldAbort())
	{
		return false;
	}
	mapping->isHidden_ = isHidden(sourceIndex, sourceModel, helper, state);

	auto rowCount = sourceModel.rowCount(sourceIndex);
	if (shouldAbort())
	{
		return false;
	}
	for (auto row = 0; row < rowCount; ++row)
	{
		auto child = new Mapping();
		child->parent_ = mapping;
		mapping->children_.emplace_back(child);
		child->indexInParent_ = row;

		auto childIndex = sourceModel.index(row, 0, sourceIndex);

		if (map(child, childIndex, sourceModel, helper, state, shouldAbort) == false)
		{
			return false;
		}
	}

	for (auto& child : mapping->children_)
	{
		for (auto& extension : child->extensions_)
		{
			mapping->extensions_[extension.first] += extension.second;
		}
	}

	if (!sourceIndex.isValid())
	{
		return true;
	}

	if (rowCount == 0 && !state.getExtension().empty())
	{
		bool ok = false;
		SharedString sourceValue;
		if (helper && helper->itemModel_)
		{
			helper->cachedHelper_.reset();
			auto sourceData =
				helper->itemModel_->variantData(
					helper->cachedHelper_, sourceIndex, Qt::DisplayRole);
			ok = sourceData.tryCast(sourceValue);
		}

		if (ok == false)
		{
			auto sourceData = sourceModel.data(sourceIndex);
			sourceValue = sourceData.toString().toUtf8().data();
		}

		auto && path = sourceValue.str();
		auto pos = path.find_last_of('.');
		if (pos == std::string::npos)
		{
			return true;
		}

		auto ext = path.c_str() + pos + 1;
		mapping->extensions_[HashUtilities::computei(ext)] = 1;
	}
	return true;
}

//--------------------------------------------------------------------------
bool ModelMapping::mapping_is_visible(const Mapping* mapping, const State& state)
{
	if (!state.getShowHidden() && mapping->isHidden_)
	{
		return false;
	}

	if (mapping->removing_)
	{
		return false;
	}

	const auto& extensionHashes = state.getExtensionHashes();
	if (extensionHashes.empty())
	{
		return true;
	}

	for (auto& extensionHash : extensionHashes)
	{
		auto it = mapping->extensions_.find(extensionHash);
		if (it != mapping->extensions_.end() && it->second > 0)
		{
			return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------------
void ModelMapping::map_insert(
	Mapping* mapping, const QModelIndex& sourceIndex, QAbstractItemModel& sourceModel, int first, int last, WgtQueryHelper * helper, const State & state)
{
	auto count = last - first + 1;
	for (auto& child : mapping->children_)
	{
		if (static_cast<int>(child->indexInParent_) >= first)
		{
			child->indexInParent_ += count;
		}
	}

	std::vector<std::unique_ptr<Mapping>> insertChildren;
	for (auto row = first; row <= last; ++row)
	{
		auto child = new Mapping();
		child->parent_ = mapping;
		insertChildren.emplace_back(child);
		child->indexInParent_ = row;

		auto childIndex = sourceModel.index(row, 0, sourceIndex);

		std::function< bool() > shouldAbort = [] { return false;  };
		map(child, childIndex, sourceModel, helper, state, shouldAbort);
	}

	auto updateMapping = mapping;
	while (updateMapping != nullptr)
	{
		for (auto& child : insertChildren)
		{
			for (auto& extension : child->extensions_)
			{
				updateMapping->extensions_[extension.first] += extension.second;
			}
		}
		updateMapping = updateMapping->parent_;
	}

	auto insertIt = mapping->children_.begin();
	std::advance(insertIt, first);
	for (auto it = insertChildren.begin(); it != insertChildren.end(); ++it)
	{
		insertIt = mapping->children_.emplace(insertIt, std::move(*it));
		++insertIt;
	}
}


//------------------------------------------------------------------------------
void ModelMapping::map_pre_remove(
	Mapping* mapping, int first, int last, WgtQueryHelper * helper)
{
	auto count = last - first + 1;
	for (auto& child : mapping->children_)
	{
		if (static_cast<int>(child->indexInParent_) > last)
		{
			child->indexInParent_ -= count;
		}
	}

	std::vector<Mapping*> removeChildren;
	auto removeIt = mapping->children_.begin();
	std::advance(removeIt, first);
	for (auto i = 0; i < count; ++i, ++removeIt)
	{
		removeIt->get()->removing_ = true;
		removeChildren.emplace_back(removeIt->get());
	}

	auto updateMapping = mapping;
	while (updateMapping != nullptr)
	{
		for (auto& child : removeChildren)
		{
			for (auto& extension : child->extensions_)
			{
				updateMapping->extensions_[extension.first] -= extension.second;
			}
		}
		updateMapping = updateMapping->parent_;
	}
}

//------------------------------------------------------------------------------
void ModelMapping::map_post_remove(
	Mapping* mapping, int first, int last, WgtQueryHelper * helper)
{
	auto count = last - first + 1;

	auto removeIt = mapping->children_.begin();
	std::advance(removeIt, first);
	for (auto i = 0; i < count; ++i)
	{
		TF_ASSERT(removeIt->get()->removing_ == true);
		removeIt = mapping->children_.erase(removeIt);
	}
}

//------------------------------------------------------------------------------
void ModelMapping::update_visible_mappings(
	const NotificationImpl & notificationImpl,
	Mapping* mapping, const State & state, bool recurse )
{
	mapping->visibleChildren_.clear();
	auto visibleIndex = 0;
	for (auto& child : mapping->children_)
	{
		auto wasVisible = child->visibleIndexInParent_ != Mapping::INVALID_INDEX;
		if (recurse)
		{
			if (wasVisible)
			{
				update_visible_mappings(notificationImpl, child.get(), state, recurse);
			}
			else
			{
				update_visible_mappings(child.get(), state, recurse);
			}
		}

		auto isVisible = mapping_is_visible(child.get(), state);
		if (wasVisible && isVisible)
		{
			mapping->visibleChildren_.emplace_back(child.get());
			++visibleIndex;
		}
		else if (!wasVisible && isVisible)
		{
			auto helper =
				notificationImpl.createHelper(visibleIndex, visibleIndex);

			for (auto& child : mapping->children_)
			{
				if (child->visibleIndexInParent_ != Mapping::INVALID_INDEX && static_cast<int>(child->visibleIndexInParent_) >= visibleIndex)
				{
					++child->visibleIndexInParent_;
				}
			}
			mapping->visibleChildren_.emplace_back(child.get());
			TF_ASSERT(child->visibleIndexInParent_ == Mapping::INVALID_INDEX);
			child->visibleIndexInParent_ = visibleIndex++;
		}
		else if (wasVisible && !isVisible)
		{
			for (auto& child : mapping->children_)
			{
				if (child->visibleIndexInParent_ != Mapping::INVALID_INDEX && static_cast<int>(child->visibleIndexInParent_) > visibleIndex)
				{
					--child->visibleIndexInParent_;
				}
			}
			auto helper =
				notificationImpl.createHelper(visibleIndex, visibleIndex);
			TF_ASSERT(child->visibleIndexInParent_ == visibleIndex);
			child->visibleIndexInParent_ = Mapping::INVALID_INDEX;
		}
		else
		{
			TF_ASSERT(child->visibleIndexInParent_ == Mapping::INVALID_INDEX);
		}
	}
}

} //end namespace AssetFilterProxyDetails


//==============================================================================
struct WGAssetFilterProxy::Impl
	: public WgtItemModelBase
	, public Depends< IBackgroundWorker >
{

typedef AssetFilterProxyDetails::ModelMapping ModelMapping;
typedef AssetFilterProxyDetails::Mapping Mapping;
typedef AssetFilterProxyDetails::State State;

public:
	Impl(WGAssetFilterProxy & proxyModel)
		: proxyModel_(proxyModel)
		, mapping_(new ModelMapping([this]() { beginRemapModel(); }, [this]() { endRemapModel(); }))
		, abortMappingDelagate_([this]()
	{
		return abortRemapping_;
	})
		, asyncMode_(false)
	{
	}

	~Impl()
	{
		{
			abortRemapping_ = true;
		}
	}


	QAbstractItemModel * getSourceModel() const override
	{
		return proxyModel_.sourceModel();
	}

	QModelIndex getSourceIndex(const QModelIndex & index) const override
	{
		return proxy_to_source(index, *getSourceModel());
	}

	//--------------------------------------------------------------------------
	QModelIndex proxy_to_source(const QModelIndex& proxyIndex, QAbstractItemModel& sourceModel) const
	{
		if (!proxyIndex.isValid())
		{
			return QModelIndex();
		}

		auto parent_mapping = reinterpret_cast<Mapping*>(proxyIndex.internalId());
		auto parent = mapping_to_source(parent_mapping, sourceModel);
		auto proxy_row = proxyIndex.row();

		TF_ASSERT(proxy_row < static_cast<int>(parent_mapping->visibleChildren_.size()));
		auto child = parent_mapping->visibleChildren_[proxy_row];
		return sourceModel.index(static_cast<int>(child->indexInParent_), proxyIndex.column(), parent);
	}


	//--------------------------------------------------------------------------
	QModelIndex source_to_proxy(const QModelIndex& sourceIndex, const WGAssetFilterProxy& proxyModel)
	{
		if (!sourceIndex.isValid())
		{
			return QModelIndex();
		}

		auto mapping = get_mapping_from_source(sourceIndex);
		if (!ModelMapping::mapping_is_visible(mapping, getCurrentState()))
		{
			return QModelIndex();
		}
		auto parent_mapping = mapping->parent_;
		TF_ASSERT(parent_mapping != nullptr);
		return proxyModel.createIndex(static_cast<int>(mapping->visibleIndexInParent_), 0, parent_mapping);
	}

	QModelIndex mapping_to_source(const Mapping* mapping, QAbstractItemModel& sourceModel) const
	{
		if (mapping == &getCurrentMapping().mapping_)
		{
			return QModelIndex();
		}

		TF_ASSERT(mapping->parent_ != nullptr);
		auto parent = mapping_to_source(mapping->parent_, sourceModel);
		return sourceModel.index(static_cast<int>(mapping->indexInParent_), 0, parent);
	}

	Mapping* get_mapping_from_source(const QModelIndex& source_index)
	{
		if (!source_index.isValid())
		{
			return &(getCurrentMapping().mapping_);
		}

		auto source_parent = source_index.parent();
		auto mapping = get_mapping_from_source(source_parent);
		if(mapping == nullptr || static_cast<int>(mapping->children_.size()) <= source_index.row())
		{
			qDebug() << "Invalid source index " << source_index;
			TF_ASSERT(!"WGAssetFilterProxy - invalid source index");
			return &(getCurrentMapping().mapping_);
		}
		return mapping->children_[source_index.row()].get();
	}

	Mapping* get_mapping_from_proxy(const QModelIndex& proxy_index)
	{
		if (!proxy_index.isValid())
		{
			return &getCurrentMapping().mapping_;
		}

		auto parent_mapping = reinterpret_cast<Mapping*>(proxy_index.internalId());
		auto proxy_row = proxy_index.row();
		return parent_mapping->visibleChildren_[proxy_row];
	}

public:
	bool abortRemapping_ = false;
	std::unique_ptr< ModelMapping > mapping_;
	bool asyncMode_;
private:
	std::unique_ptr< ModelMapping > mappingInFlight_;
	WGAssetFilterProxy & proxyModel_;
	std::function< bool() > abortMappingDelagate_;

public:
	const ModelMapping & getCurrentMapping() const
	{
		return *mapping_;
	}

	ModelMapping & getCurrentMapping()
	{
		return *mapping_;
	}

	const State & getCurrentState() const
	{
		return mapping_->getState();
	}

	/*void remapModel()
	{
		{
			std::lock_guard< std::mutex > guard(abortMudex_);
			abortRemapping_ = true;
		}
		auto job = [this]()
		{
			{
				std::lock_guard< std::mutex > guard(abortMudex_);
				abortRemapping_ = false;
			}
			std::lock_guard< std::mutex > guard(mappingMutex_);
			auto newModel = mapping_->getState().getSourceModel();
			if (newModel != nullptr)
			{
				mappingInFlight_.reset(new ModelMapping(*mapping_));
				if (mappingInFlight_->map(*newModel, abortMappingDelagate_) == false)
				{
					return;
				}
			}
			else
			{
				mappingInFlight_.reset();
			}

			{
				std::lock_guard< std::recursive_mutex > swapGuard(swapMappingMutex_);
				auto prevModel = proxyModel_.QAbstractProxyModel::sourceModel();
				resetConnections(newModel);
				proxyModel_.beginResetModel();
				if (mappingInFlight_ != nullptr)
				{
					mapping_.swap(mappingInFlight_);
					mappingInFlight_.reset();
				}
				if (prevModel != newModel)
				{
					proxyModel_.QAbstractProxyModel::setSourceModel(newModel);
				}
			}
			proxyModel_.endResetModel();
		};
		auto backgroundWorker = get< IBackgroundWorker >();
		if (backgroundWorker && asyncMode_)
		{
			backgroundWorker->queueBackgroundJob(job);
		}
		else
		{
			job();
		}
	}*/

	void beginRemapModel()
	{
		proxyModel_.beginResetModel();
	}

	void endRemapModel()
	{
		mapping_.reset(new ModelMapping(*mapping_));
		auto newModel = mapping_->getState().getSourceModel();
		if (newModel != nullptr)
		{
			auto res = mapping_->map(*newModel, abortMappingDelagate_);
			TF_ASSERT(res);
		}
		auto prevModel = proxyModel_.QAbstractProxyModel::sourceModel();
		if (prevModel != newModel)
		{
			proxyModel_.QAbstractProxyModel::setSourceModel(newModel);
			resetConnections(newModel);
		}
		proxyModel_.endResetModel();
	}

private:
	void resetConnections( QAbstractItemModel * model )
	{
		connections_.reset();

		if (model == nullptr)
		{
			return;
		}

		/*connections_ +=
			QObject::connect(model, &QAbstractItemModel::modelReset, [this]() {
			remapModel();
		});*/

		connections_ +=
			QObject::connect(model, &QAbstractItemModel::modelAboutToBeReset, [this]() {
			beginRemapModel();
		});
		connections_ +=
			QObject::connect(model, &QAbstractItemModel::modelReset, [this]() {
			endRemapModel();
		});

		connections_ += QObject::connect(model, &QAbstractItemModel::dataChanged, &proxyModel_, &WGAssetFilterProxy::onSourceDataChanged);
		connections_ += QObject::connect(model, &QAbstractItemModel::rowsAboutToBeInserted, &proxyModel_, &WGAssetFilterProxy::onSourceRowsAboutToBeInserted);
		connections_ += QObject::connect(model, &QAbstractItemModel::rowsInserted, &proxyModel_, &WGAssetFilterProxy::onSourceRowsInserted);
		connections_ += QObject::connect(model, &QAbstractItemModel::rowsAboutToBeRemoved, &proxyModel_, &WGAssetFilterProxy::onSourceRowsAboutToBeRemoved);
		connections_ += QObject::connect(model, &QAbstractItemModel::rowsRemoved, &proxyModel_, &WGAssetFilterProxy::onSourceRowsRemoved);

		connections_ += QObject::connect(model, &QAbstractItemModel::columnsAboutToBeInserted,
			[this](const QModelIndex& parent, int first, int last) {
			proxyModel_.beginInsertColumns(QModelIndex(), first, last);
		});
		connections_ += QObject::connect(model, &QAbstractItemModel::columnsInserted,
			[this](const QModelIndex& parent, int first, int last) {
			proxyModel_.endInsertColumns();
		});
		connections_ += QObject::connect(model, &QAbstractItemModel::columnsAboutToBeRemoved,
			[this](const QModelIndex& parent, int first, int last) {
			proxyModel_.beginRemoveColumns(QModelIndex(), first, last);
		});
		connections_ += QObject::connect(model, &QAbstractItemModel::columnsRemoved,
			[this](const QModelIndex& parent, int first, int last) {
			proxyModel_.endRemoveColumns();
		});
	}

	QtConnectionHolder connections_;
};


//==============================================================================
WGAssetFilterProxy::WGAssetFilterProxy()
	: WGTInterfaceProvider( this )
	, impl_(new Impl( *this ))
{
	registerInterface(*impl_);
}


//------------------------------------------------------------------------------
WGAssetFilterProxy::~WGAssetFilterProxy()
{
}


//------------------------------------------------------------------------------
void WGAssetFilterProxy::setSourceModel(QAbstractItemModel* model)
{
	auto stateChanger = impl_->getCurrentState().getStateChanger();
	if (stateChanger.setSourceModel(model) == false)
	{
		return;
	}
}


//------------------------------------------------------------------------------
QModelIndex WGAssetFilterProxy::mapToSource(const QModelIndex& proxyIndex) const
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return QModelIndex();
	}

	return impl_->proxy_to_source(proxyIndex, *source);
}

QModelIndex WGAssetFilterProxy::mapFromSource(const QModelIndex& sourceIndex) const
{
	return impl_->source_to_proxy(sourceIndex, *this);
}


//------------------------------------------------------------------------------
QModelIndex WGAssetFilterProxy::index(int row, int column, const QModelIndex& parent) const
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return QModelIndex();
	}	

	auto parent_mapping = impl_->get_mapping_from_proxy(parent);
	return createIndex(row, column, parent_mapping);
}


//------------------------------------------------------------------------------
QModelIndex WGAssetFilterProxy::parent(const QModelIndex& child) const
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return QModelIndex();
	}

	auto mapping = reinterpret_cast<AssetFilterProxyDetails::Mapping*>(child.internalId());
	auto parent_mapping = mapping->parent_;
	if (parent_mapping == nullptr ||
		mapping->visibleIndexInParent_ == Impl::Mapping::INVALID_INDEX)
	{
		return QModelIndex();
	}

	return createIndex(static_cast<int>(mapping->visibleIndexInParent_), 0, parent_mapping);
}


//------------------------------------------------------------------------------
int WGAssetFilterProxy::rowCount(const QModelIndex& parent) const
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return 0;
	}

	auto parent_mapping = impl_->get_mapping_from_proxy(parent);
	TF_ASSERT(parent_mapping != nullptr);
	return static_cast<int>(parent_mapping->visibleChildren_.size());
}


//------------------------------------------------------------------------------
int WGAssetFilterProxy::columnCount(const QModelIndex& parent) const
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return 0;
	}

	return source->columnCount();
}


//------------------------------------------------------------------------------
bool WGAssetFilterProxy::hasChildren(const QModelIndex& parent) const
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return false;
	}

	auto parent_mapping = impl_->get_mapping_from_proxy(parent);
	TF_ASSERT(parent_mapping != nullptr);
	return !parent_mapping->visibleChildren_.empty();
}


//------------------------------------------------------------------------------
QVariant WGAssetFilterProxy::headerData(int section, Qt::Orientation orientation, int role) const
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return QVariant();
	}

	return source->headerData(section, orientation, role);
}


//------------------------------------------------------------------------------
bool WGAssetFilterProxy::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return false;
	}

	return source->setHeaderData(section, orientation, value, role);
}


//------------------------------------------------------------------------------
QHash<int, QByteArray> WGAssetFilterProxy::roleNames() const
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return QHash<int, QByteArray>();
	}

	return source->roleNames();
}


//------------------------------------------------------------------------------
QString WGAssetFilterProxy::getExtension() const
{
	return impl_->getCurrentState().getExtension().c_str();
}

//------------------------------------------------------------------------------
bool WGAssetFilterProxy::getShowHidden() const
{
	return impl_->getCurrentState().getShowHidden();
}

//------------------------------------------------------------------------------
void WGAssetFilterProxy::setExtension(const QString& extension)
{
	std::string data = extension.toUtf8().data();
	{
		auto stateChanger = impl_->getCurrentState().getStateChanger();
		if (!stateChanger.setExtension(data))
		{
			return;
		}
	}
	emit extensionChanged();
}

//------------------------------------------------------------------------------
void WGAssetFilterProxy::setShowHidden(bool showHidden)
{
	{
		auto stateChanger = impl_->getCurrentState().getStateChanger();
		if (!stateChanger.setShowHidden(showHidden))
		{
			return;
		}
	}
	emit showHiddenChanged();
}


//------------------------------------------------------------------------------
void WGAssetFilterProxy::onSourceDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
	for (auto roleId : roles)
	{
		if (roleId == ItemRole::displayId)
		{
			//TODO: These functions shouldn't be getting called if display role changes - if they start to show up in the log then we will need to fill out their logic
			NGT_WARNING_MSG("WGAssetFilterProxy::onSourceDataChanged - display role change not supported.");
			return;
		}
	}
	QModelIndex proxy_topLeft = mapFromSource(topLeft);
	QModelIndex proxy_bottomRight = mapFromSource(bottomRight);
	emit dataChanged(proxy_topLeft, proxy_bottomRight, roles);
}


//------------------------------------------------------------------------------
void WGAssetFilterProxy::onSourceRowsAboutToBeInserted(const QModelIndex& parent, int first, int last)
{
}


//------------------------------------------------------------------------------
void WGAssetFilterProxy::onSourceRowsInserted(const QModelIndex& parent, int first, int last)
{
	using namespace AssetFilterProxyDetails;
	auto& source_model = *sourceModel();
	auto interfaceProvider =
		IWGTInterfaceProvider::getInterfaceProvider(source_model);
	ModelMapping::WgtQueryHelper helper;
	helper.itemModel_ =
		interfaceProvider ? interfaceProvider->queryInterface< IWgtItemModel >() : nullptr;
	helper.itemModel_ = helper.itemModel_&& helper.itemModel_->canUse(helper.cachedHelper_) ? helper.itemModel_ : nullptr;

	auto mapping = impl_->get_mapping_from_source(parent);
	TF_ASSERT(mapping != nullptr);

	ModelMapping::map_insert(mapping, parent, source_model, first, last, &helper, impl_->getCurrentState());

	ModelMapping::update_visible_mappings(NotificationImpl(
		[this, mapping](int first, int last)
	{
		auto row = static_cast<int>(mapping->indexInParent_);
		auto data = mapping->parent_;
		auto proxyIndex = createIndex(row, 0, data);
		beginInsertRows(proxyIndex, first, last);
	},
		[this]()
	{
		endInsertRows();
	}), mapping, impl_->getCurrentState(), true);
}


//------------------------------------------------------------------------------
void WGAssetFilterProxy::onSourceRowsAboutToBeRemoved(
	const QModelIndex& parent, int first, int last)
{
	using namespace AssetFilterProxyDetails;

	auto& source_model = *sourceModel();
	auto interfaceProvider =
		IWGTInterfaceProvider::getInterfaceProvider(source_model);
	ModelMapping::WgtQueryHelper helper;
	helper.itemModel_ =
		interfaceProvider ? interfaceProvider->queryInterface< IWgtItemModel >() : nullptr;
	helper.itemModel_ = helper.itemModel_&& helper.itemModel_->canUse(helper.cachedHelper_) ? helper.itemModel_ : nullptr;

	auto mapping = impl_->get_mapping_from_source(parent);
	TF_ASSERT(mapping != nullptr);
	ModelMapping::map_pre_remove(mapping, first, last, &helper);
	//ModelMapping::update_visible_mappings(NotificationImpl(
	//	[this, mapping]( int first, int last )
	//	{
	//		SCOPE_TAG
	//		auto row = static_cast<int>(mapping->indexInParent_);
	//		auto data = mapping->parent_;
	//		auto proxyIndex = createIndex(row, 0, data);
	//		beginRemoveRows(proxyIndex, first, last);
	//	},
	//	[this]()
	//	{
	//		endRemoveRows();
	//	}), mapping, impl_->getCurrentState(), true );
}

//------------------------------------------------------------------------------
void WGAssetFilterProxy::onSourceRowsRemoved(
	const QModelIndex& parent, int first, int last)
{
	using namespace AssetFilterProxyDetails;

	auto& source_model = *sourceModel();
	auto interfaceProvider =
		IWGTInterfaceProvider::getInterfaceProvider(source_model);
	ModelMapping::WgtQueryHelper helper;
	helper.itemModel_ =
		interfaceProvider ? interfaceProvider->queryInterface< IWgtItemModel >() : nullptr;
	helper.itemModel_ = helper.itemModel_&& helper.itemModel_->canUse(helper.cachedHelper_) ? helper.itemModel_ : nullptr;

	auto mapping = impl_->get_mapping_from_source(parent);
	TF_ASSERT(mapping != nullptr);

	ModelMapping::update_visible_mappings(NotificationImpl(
		[this, mapping](int first, int last)
	{
		SCOPE_TAG
			auto row = static_cast<int>(mapping->indexInParent_);
		auto data = mapping->parent_;
		auto proxyIndex = createIndex(row, 0, data);
		beginRemoveRows(proxyIndex, first, last);
	},
		[this]()
	{
		endRemoveRows();
	}), mapping, impl_->getCurrentState(), true);
	ModelMapping::map_post_remove(mapping, first, last, &helper);
}
}