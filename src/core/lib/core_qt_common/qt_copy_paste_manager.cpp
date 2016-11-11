#include "qt_copy_paste_manager.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_copy_paste/i_copyable_object.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_serialization/resizing_memory_stream.hpp"
#include "core_serialization/fixed_memory_stream.hpp"
#include "core_serialization/serializer/xml_serializer.hpp"
#include "core_variant/collection.hpp"

#include <QtGui/QClipboard>
#include <QApplication>
#include <QMimeData>

namespace wgt
{
namespace
{
const char* s_ValueHintTag = "DATAHINT";
const char* s_ValueTag = "COPYDATA";
}

//==============================================================================
QtCopyPasteManager::QtCopyPasteManager()
    : clipboard_(QApplication::clipboard()), definitionManager_(nullptr), commandManager_(nullptr)
{
	QObject::connect(clipboard_, &QClipboard::dataChanged, this, &QtCopyPasteManager::clipboardChangedSignal);
}

//==============================================================================
QtCopyPasteManager::~QtCopyPasteManager()
{
}

//==============================================================================
void QtCopyPasteManager::onSelect(ICopyableObject* pObject, bool append)
{
	assert(pObject != nullptr);
	if (!append)
	{
		curObjects_.clear();
	}
	curObjects_.push_back(pObject);
}

//==============================================================================
void QtCopyPasteManager::onDeselect(ICopyableObject* pObject, bool reset)
{
	if (reset)
	{
		curObjects_.clear();
		return;
	}
	curObjects_.erase(std::remove(curObjects_.begin(), curObjects_.end(), pObject), curObjects_.end());
}

//==============================================================================
bool QtCopyPasteManager::copy()
{
	assert(!curObjects_.empty());
	assert(definitionManager_);

	ResizingMemoryStream dataStream;
	XMLSerializer serializer(dataStream, *definitionManager_);

	bool ret = true;
	serializer.serialize(curObjects_.size());
	std::vector<ICopyableObject*>::iterator iter;
	for (iter = curObjects_.begin(); iter != curObjects_.end() && ret; ++iter)
	{
		assert(*iter != nullptr);
		const char* hint = (*iter)->getDataHint();
		const Variant& value = (*iter)->getData();
		if (strcmp(hint, "") != 0)
		{
			serializer.serialize(s_ValueHintTag);
			serializer.serialize(hint);
		}
		serializer.serialize(s_ValueTag);
		ret = serializer.serialize(value);
	}
	if (!ret)
	{
		assert(false);
		return false;
	}

	// copy data to clipboard
	serializer.sync();
	clipboard_->setText(QString::fromStdString(dataStream.buffer()));

	return ret;
}

//==============================================================================
bool QtCopyPasteManager::paste()
{
	assert(!curObjects_.empty());
	assert(definitionManager_);
	assert(commandManager_);

	// get data from clipboard
	QString data = clipboard_->text();
	QByteArray data_utf8 = data.toUtf8();
	// if nothing is in clipboard, do nothing
	if (data_utf8.isEmpty())
	{
		return false;
	}

	FixedMemoryStream dataStream(data_utf8.constData(), data_utf8.size());
	XMLSerializer serializer(dataStream, *definitionManager_);

	// deserialize values
	std::string tag;
	std::string hint;
	std::string valueTag;
	std::vector<Variant> values;
	size_t objectCount = 0;
	if (!serializer.deserialize(objectCount))
	{
		return false;
	}

	for (; objectCount > 0; --objectCount)
	{
		serializer.deserialize(tag);
		if (tag == s_ValueHintTag)
		{
			serializer.deserialize(hint);
			serializer.deserialize(valueTag);
			assert(valueTag == s_ValueTag);
		}
		else
		{
			assert(tag == s_ValueTag);
		}

		Variant v;
		if (!serializer.deserialize(v))
		{
			assert(false);
			return false;
		}
		values.emplace_back(std::move(v));
	}

	if (values.empty())
	{
		return false;
	}
	// paste value
	commandManager_->beginBatchCommand();
	bool bSuccess = false;
	std::vector<ICopyableObject*>::iterator iter;
	for (iter = curObjects_.begin(); iter != curObjects_.end(); ++iter)
	{
		assert(*iter != nullptr);
		Variant value = (*iter)->getData();

		if (value.typeIs<Collection>())
		{
			Collection collection;
			bool isOk = value.tryCast(collection);
			if (!isOk)
			{
				break;
			}
			size_t i = 0;
			for (auto& v : values)
			{
				assert(i < collection.size());
				collection[i++] = v;
			}
			value = collection;
		}
		else
		{
			value = values[0];
		}
		if ((*iter)->setData(value))
		{
			bSuccess = true;
		}
	}
	if (bSuccess)
	{
		commandManager_->endBatchCommand();
	}
	else
	{
		commandManager_->abortBatchCommand();
	}

	return bSuccess;
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
bool QtCopyPasteManager::canCopy() const
{
	return clipboard_ && !curObjects_.empty();
}

//==============================================================================
bool QtCopyPasteManager::canPaste() const
{
	return clipboard_ && !curObjects_.empty();
}

//==============================================================================
void QtCopyPasteManager::init(IDefinitionManager* definitionManager, ICommandManager* commandManager)
{
	curObjects_.clear();
	definitionManager_ = definitionManager;
	commandManager_ = commandManager;
}

//==============================================================================
void QtCopyPasteManager::fini()
{
	curObjects_.clear();
	definitionManager_ = nullptr;
	commandManager_ = nullptr;
}
//==============================================================================
void QtCopyPasteManager::clipboardChangedSignal()
{
	onClipboardDataChanged();
}

} // end namespace wgt
