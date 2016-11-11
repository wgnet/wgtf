/**
 * WGFileDialogQI
 *
 * The Qml Invokable FileDialog class.
 *
 */

#ifndef WG_FILEDIALO_QI_HPP
#define WG_FILEDIALO_QI_HPP

#include "../qt_new_handler.hpp"
#include <QFileDialog>
#include <QObject>
#include <QStringList>

class QFileDialog;

namespace wgt
{
class WGFileDialogQI : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString title MEMBER title_ WRITE setTitle)
	Q_PROPERTY(QString filter MEMBER filter_ WRITE setFilter)
	Q_PROPERTY(int fileModeFlag MEMBER fileModeFlag_ WRITE setFileModeFlag)
	Q_PROPERTY(int mode MEMBER mode_ WRITE setMode)

	Q_PROPERTY(QStringList fileNames READ getFileNames NOTIFY fileNamesChanged REVISION 1)

public:
	WGFileDialogQI(QObject* parent = NULL);
	~WGFileDialogQI();

	Q_INVOKABLE void componentOnComplete();

	Q_INVOKABLE void setTitle(QString titile);
	Q_INVOKABLE void setFilter(QString filter);
	Q_INVOKABLE void setFileModeFlag(int fileModeFlag);
	Q_INVOKABLE void setMode(int mode);

	Q_INVOKABLE bool showDialog();

	DECLARE_QT_MEMORY_HANDLER

signals:
	Q_REVISION(1)
	void fileNamesChanged();

private:
	QString title_;
	QString filter_;
	int mode_;
	int fileModeFlag_;

	QStringList fileNames_;
	QFileDialog* fileDialog_;

	QStringList getFileNames();
	void createFileDialog();
};
} // end namespace wgt
#endif // WG_FILEDIALO_QI_HPP
