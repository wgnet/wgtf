#include "qt_copy_paste_manager.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_serialization/resizing_memory_stream.hpp"
#include "core_serialization/fixed_memory_stream.hpp"
#include "core_serialization_xml/xml_serializer.hpp"
#include "core_variant/collection.hpp"

#include <QtGui/QClipboard>
#include <QApplication>
#include <QMimeData>

namespace wgt
{

//==============================================================================
QtCopyPasteManager::QtCopyPasteManager() : clipboard_(QApplication::clipboard())
{
	QObject::connect(clipboard_, &QClipboard::dataChanged, this, &QtCopyPasteManager::clipboardChangedSignal);
}

//==============================================================================
QtCopyPasteManager::~QtCopyPasteManager()
{
}

//==============================================================================
MimeData QtCopyPasteManager::getClipboardContents()
{
	MimeData result;

	const QMimeData* mimeData = clipboard_->mimeData();
	QStringList formatList = mimeData->formats();

	for (auto& format : formatList)
	{
		QByteArray data = mimeData->data(format);
		std::vector<char> strData(data.begin(), data.end());
		result[format.toUtf8().data()] = strData;
	}

	return result;
}

//==============================================================================
void QtCopyPasteManager::setClipboardContents(MimeData& mimeData)
{
	// Ownership is transferred to the clipboard when assigned
	QMimeData* qMimeData = new QMimeData();

	for (auto& formats : mimeData)
	{
		QString format = formats.first.c_str();
		QByteArray data(formats.second.data(), (int)formats.second.size());
		qMimeData->setData(format, data);
	}

	clipboard_->setMimeData(qMimeData);
}

//==============================================================================
std::string QtCopyPasteManager::getText()
{
	QString text = clipboard_->text();
	std::string result = text.toUtf8().data();
	return result;
}

//==============================================================================
void QtCopyPasteManager::setText(std::string str)
{
	clipboard_->setText(QString::fromStdString(str));
}
//==============================================================================

Connection QtCopyPasteManager::connectClipboardDataChanged(ClipboardCallback callback)
{
	Connection result = onClipboardDataChanged.connect(callback);
	return result;
}

//==============================================================================
void QtCopyPasteManager::clipboardChangedSignal()
{
	onClipboardDataChanged();
}

} // end namespace wgt
