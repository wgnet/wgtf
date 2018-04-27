#pragma once

#include "core_qt_common/qt_filter_object.hpp"
#include "core_data_model/i_item_role.hpp"

namespace wgt
{
class FolderFilterObject
	: public QtFilterObject
{
public:
	Q_OBJECT
	DECLARE_QT_MEMORY_HANDLER

	enum FolderFilterRole
	{
		Directory = 0,
		Folder = 1
	};
	Q_ENUMS(FolderFilterRole)

	Q_PROPERTY(FolderFilterRole filterRole READ getFilterRole WRITE setFilterRole)
	Q_PROPERTY(QString filterName READ getFilterName WRITE setFilterName)

	FolderFilterObject();
	~FolderFilterObject();

	bool filterAcceptsRowValid() const override;
	bool filterAcceptsRow(const Variant& variant) const override;

protected:
	QString getFilterName() const override;
	void setFilterName(const QString& filterName);

	FolderFilterRole getFilterRole() const;
	void setFilterRole(FolderFilterRole filterRole);

private:
	ItemRole::Id filterRoleId_;
	FolderFilterRole filterRole_;
	std::string filterName_;
};
}
