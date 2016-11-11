#include "qt_helpers.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_qt_common/i_qt_type_converter.hpp"

#include <set>

#include <QQmlEngine>
#include <QQuickItem>
#include <QWindow>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include "core_variant/variant.hpp"
#include "core_logging/logging.hpp"
#include "core_string_utils/string_utils.hpp"
#include "core_string_utils/file_path.hpp"

namespace wgt
{
namespace QtHelpers
{
static IQtFramework* s_qtFramework = nullptr;

//==============================================================================
QVariant toQVariant(const Variant& variant, QObject* parent)
{
	if (s_qtFramework == nullptr)
	{
		s_qtFramework = Context::queryInterface<IQtFramework>();
	}
	if (s_qtFramework != nullptr)
	{
		return s_qtFramework->toQVariant(variant, parent);
	}

	return QVariant(QVariant::Invalid);
}

//==============================================================================
QVariant toQVariant(const ObjectHandle& object, QObject* parent)
{
	return toQVariant(Variant(object), parent);
}

//==============================================================================
Variant toVariant(const QVariant& qVariant)
{
	Variant variant;

	if (s_qtFramework == nullptr)
	{
		s_qtFramework = Context::queryInterface<IQtFramework>();
	}
	if (s_qtFramework != nullptr)
	{
		return s_qtFramework->toVariant(qVariant);
	}

	return Variant();
}

//==============================================================================
QQuickItem* findChildByObjectName(QObject* parent, const char* controlName)
{
	std::list<QObject*> queue;
	queue.push_back(parent);

	QString targetName(controlName);

	std::set<QObject*> visited;
	while (queue.empty() == false)
	{
		QQuickItem* child = qobject_cast<QQuickItem*>(queue.front());
		if (child == NULL)
		{
			QWindow* window = qobject_cast<QWindow*>(queue.front());
			queue.pop_front();
			if (window == NULL)
			{
				continue;
			}
			const QObjectList& children = window->children();
			for (QObjectList::const_iterator it = children.begin(); it != children.end(); ++it)
			{
				std::pair<std::set<QObject*>::iterator, bool> insertIt = visited.insert(*it);
				if (insertIt.second)
				{
					queue.push_back(*it);
				}
			}
			continue;
		}
		if (targetName == child->objectName())
		{
			return child;
		}
		queue.pop_front();

		QList<QQuickItem*> children = child->childItems();
		for (QList<QQuickItem*>::const_iterator it = children.begin(); it != children.end(); ++it)
		{
			std::pair<std::set<QObject*>::iterator, bool> insertIt = visited.insert(*it);
			if (insertIt.second)
			{
				queue.push_back(*it);
			}
		}
	}
	return NULL;
}

std::string removeQRCPrefix(const char* path)
{
	std::string filepath(path);
	StringUtils::erase_string(filepath, "qrc:");
	StringUtils::erase_string(filepath, ":");

	if (!filepath.empty())
	{
		int index = 0;
		while (filepath[index] == FilePath::kDirectorySeparator || filepath[index] == FilePath::kAltDirectorySeparator)
		{
			++index;
		}

		if (index > 0)
		{
			filepath.erase(0, index);
		}
	}

	return filepath;
}

QString resolveFilePath(const QQmlEngine& qmlEngine, const char* relativePath)
{
	if (relativePath == nullptr)
	{
		NGT_ERROR_MSG("QtHelpers::resolveFilePath(): relativePath is NULL.\n");
		return QString();
	}

	const std::string filepath(removeQRCPrefix(relativePath));

	QStringList paths = qmlEngine.importPathList();
	for (auto path : paths)
	{
		QFileInfo info(QDir(path), filepath.c_str());
		if (info.exists() && info.isFile())
		{
			return info.canonicalFilePath();
		}
	}

	return QString(relativePath);
}

QUrl resolveQmlPath(const QQmlEngine& qmlEngine, const char* relativePath)
{
	QUrl url;

	if (relativePath == nullptr)
	{
		NGT_ERROR_MSG("QtHelpers::resolveQmlPath(): relativePath is NULL.\n");
		return url;
	}

	const std::string filepath(removeQRCPrefix(relativePath));

	QStringList paths = qmlEngine.importPathList();
	for (auto path : paths)
	{
		QFileInfo info(QDir(path), filepath.c_str());
		if (info.exists() && info.isFile())
		{
			url = QUrl::fromLocalFile(info.canonicalFilePath());
			break;
		}
	}

	// fallback to qrc
	if (url.isEmpty())
	{
		QString filePath = relativePath[0] != '/' ? QString("/") + relativePath : relativePath;
		QFile file(QString(":") + filePath);
		if (file.exists())
		{
			url.setScheme("qrc");
			url.setPath(filePath);
		}
	}

	return url;
}
};
} // end namespace wgt
