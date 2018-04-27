#include "search_filter_object.hpp"
#include "core_data_model/abstract_item.hpp"
#include "core_string_utils/file_path.hpp"
#include "core_logging/logging.hpp"

namespace wgt
{
ITEMROLE(fullPath)
ITEMROLE(indexPath)

//------------------------------------------------------------------------------
SearchFilterObject::SearchFilterObject()
	: QtFilterObject(false)
{
	setFilterRole(FullPath);
}

//------------------------------------------------------------------------------
SearchFilterObject::~SearchFilterObject()
{
}

//------------------------------------------------------------------------------
bool SearchFilterObject::filterAcceptsRowValid() const
{
	return true;
}

//------------------------------------------------------------------------------
bool SearchFilterObject::hasFilter() const 
{ 
	return !filterString_.empty() || !filterExpression_.isEmpty(); 
}

//------------------------------------------------------------------------------
bool SearchFilterObject::filterAcceptsRow(const Variant& variant) const
{
	if (!hasFilter())
	{
		return true;
	}

	auto item = reinterpret_cast<AbstractItem*>(variant.value<intptr_t>());
	auto treeItem = dynamic_cast<AbstractTreeItem *>(item);
	if(!treeItem)
	{
		return false;
	}

	auto data = treeItem->getData(0, filterRoleId_);
	const bool useParentPath = parentFilter_ && !parentFilter_->filterAcceptsRow(variant);

	SharedString fullPath;
	if(data.tryCast(fullPath))
	{
		if (!filterString_.empty())
		{
			return std::regex_search(useParentPath ? getParentPath(fullPath.str()) : fullPath.str(), filter_);
		}

		if (!filterExpression_.isEmpty())
		{
			return filterExpression_.match(useParentPath ? getParentPath(fullPath.str()) : fullPath.str());
		}

		return false;
	}

	std::string strPath;
	if(data.tryCast(strPath))
	{
		if (!filterString_.empty())
		{
			return std::regex_search(useParentPath ? getParentPath(strPath) : strPath, filter_);
		}

		if (!filterExpression_.isEmpty())
		{
			return filterExpression_.match(useParentPath ? getParentPath(strPath) : strPath);
		}

		return false;
	}

	return false;
}

//------------------------------------------------------------------------------
std::string SearchFilterObject::getParentPath(const std::string& path) const
{
	auto index = path.find_last_of(FilePath::kDirectorySeparator);
	if(index == std::string::npos)
	{
		index = path.find_last_of(FilePath::kAltDirectorySeparator);
	}
	return index != std::string::npos ? path.substr(0, index + 1) : path;
}

//------------------------------------------------------------------------------
QString SearchFilterObject::getFilterString() const
{
	return QString(filterString_.c_str());
}

//------------------------------------------------------------------------------
void SearchFilterObject::setFilterString(const QString& filterString)
{
	filterString_ = filterString.toUtf8().data();
	filter_ = std::regex(filterString_.c_str(), std::regex::icase);
	emit filterStringChanged();
}

//------------------------------------------------------------------------------
QString SearchFilterObject::getFilterExpression() const
{
	return QString::fromUtf8(filterExpression_.getExpression().c_str());
}

//------------------------------------------------------------------------------
void SearchFilterObject::setFilterExpression(const QString& expression)
{
	QByteArray expressionUtf8 = expression.toUtf8();
	StringRef expressionRef{ expressionUtf8.data(), static_cast<StringRef::size_type>(expressionUtf8.length()) };
	if (StringRef(filterExpression_.getExpression()) == expressionRef)
	{
		return;
	}

	filterExpression_.reset(expressionRef);

	emit filterExpressionChanged();
}

//------------------------------------------------------------------------------
void SearchFilterObject::setFilterName(const QString& filterName)
{
	filterName_ = filterName.toUtf8().data();
}

//------------------------------------------------------------------------------
QString SearchFilterObject::getFilterName() const
{
	return QString(filterName_.c_str());
}

//------------------------------------------------------------------------------
QObject* SearchFilterObject::getParentFilter() const
{
	return parentFilter_;
}

//------------------------------------------------------------------------------
void SearchFilterObject::setParentFilter(QObject* filter)
{
	parentFilter_ = dynamic_cast<QtFilterObject*>(filter);

	if(filter && !parentFilter_)
	{
		NGT_ERROR_MSG("SearchFilterObject: parent filter must be native");
	}

	if(parentFilter_ && !parentFilter_->filterAcceptsRowValid())
	{
		NGT_ERROR_MSG("SearchFilterObject: parent filter must have valid filterAcceptsRow");
		parentFilter_ = nullptr;
	}
}

//-----------------------------------------------------------------------------
SearchFilterObject::SearchFilterRole SearchFilterObject::getFilterRole() const
{
	return filterRole_;
}

//-----------------------------------------------------------------------------
void SearchFilterObject::setFilterRole(SearchFilterRole filterRole)
{
	filterRole_ = filterRole;
	switch (filterRole_)
	{
	case SearchFilterRole::FullPath:
		filterRoleId_ = ItemRole::fullPathId;
		break;
	case SearchFilterRole::IndexPath:
		filterRoleId_ = ItemRole::indexPathId;
		break;
	default:
		filterRoleId_ = ItemRole::fullPathId;
		NGT_ERROR_MSG("SearchFilterObject: Unknown filter role set");
	}
}
}

