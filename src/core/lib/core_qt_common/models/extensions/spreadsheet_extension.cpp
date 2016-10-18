#include "spreadsheet_extension.hpp"

namespace wgt
{
ITEMROLE( dirty )
ITEMROLE( lastEdit )

SpreadsheetExtension::SpreadsheetExtension()
{
	roles_.push_back( ItemRole::dirtyName );
	roles_.push_back( ItemRole::lastEditName );
}


SpreadsheetExtension::~SpreadsheetExtension()
{
}


void SpreadsheetExtension::onDataChanged( const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles )
{
	bool modelDataChanged = roles.empty();
	for (auto role: roles)
	{
		if (role < RoleProvider::DynamicRole)
		{
			modelDataChanged = true;
			break;
		}

		ItemRole::Id roleId;
		if (extensionData_->decodeRole( role, roleId ))
		{
			modelDataChanged = true;
			break;
		}
	}
	if (!modelDataChanged)
	{
		return;
	}

	if (topLeft != bottomRight)
	{
		// TODO: iterate between topLeft & bottomRight
		return;
	}
	auto index = topLeft;

	QTime lastEdit;
	auto it = lastEdits_.find( index );
	if (it != lastEdits_.end())
	{
		lastEdit = it.value();
	}
	QVector< ItemRole::Id > extRoles;
	extRoles.append( ItemRole::lastEditId );
	if (!lastEdit.isValid() || lastEdit < commitTime_)
	{
		extRoles.append( ItemRole::dirtyId );
	}
	lastEdits_[topLeft] = QTime::currentTime();
	emit extensionData_->dataExtChanged( topLeft, bottomRight, extRoles );
}


QVariant SpreadsheetExtension::data( const QModelIndex &index, ItemRole::Id roleId ) const
{
	auto model = index.model();
	if (model == nullptr)
	{
		return QVariant::Invalid; 
	}

	if (roleId == ItemRole::dirtyId)
	{
		auto it = lastEdits_.find( index );
		if (it != lastEdits_.end())
		{
			return it.value() > commitTime_;
		}
		return false;
	}
	else if (roleId == ItemRole::lastEditId)
	{
		auto it = lastEdits_.find( index );
		if (it != lastEdits_.end())
		{
			return it.value();
		}
		return QTime();
	}

	return QVariant::Invalid;
}


QTime SpreadsheetExtension::currentTime()
{
	return QTime::currentTime();
}


void SpreadsheetExtension::commitData()
{
	QVector< ItemRole::Id > extRoles;
	extRoles.append( ItemRole::dirtyId );

	auto prevCommitTime = commitTime_;
	commitTime_ = QTime::currentTime();

	for (auto it = lastEdits_.begin(); it != lastEdits_.end(); ++it)
	{
		if (it.value() > prevCommitTime)
		{
			QModelIndex index = it.key();
			if (index.isValid())
			{
				emit extensionData_->dataExtChanged( index, index, extRoles );
			}
		}
	}
}
} // end namespace wgt
