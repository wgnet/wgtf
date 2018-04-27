#pragma once

#include "core_dependency_system/depends.hpp"
#include <QtQuick/QQuickItem>
#include <QDir>

namespace wgt
{
class QtFilePathHelper : public QQuickItem, Depends<class IFileSystem>
{
	Q_OBJECT

public:
	QtFilePathHelper();

	Q_PROPERTY(QString applicationFolder MEMBER applicationFolder_ CONSTANT)
	Q_INVOKABLE bool fileExists(const QString& file) const;
	Q_INVOKABLE bool filesExist(QString files) const;
	Q_INVOKABLE bool folderExists(const QString& folder) const;
	Q_INVOKABLE bool createFolder(const QString& folder) const;
	Q_INVOKABLE QStringList getFilesInFolder(const QString& folder, const QStringList& filters) const;

	static std::string removeQRCPrefix(const char* path);

private:
	QString applicationFolder_;
};
} // end namespace wgt