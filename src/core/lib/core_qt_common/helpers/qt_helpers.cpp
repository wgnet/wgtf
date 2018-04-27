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

namespace wgt
{
//------------------------------------------------------------------------------
QtHelpers::~QtHelpers()
{
}

//==============================================================================
QVariant QtHelpers::toQVariant(const Variant& variant, QObject* parent)
{
	auto qtFramework = get<IQtFramework>();

	if (qtFramework != nullptr)
	{
		return qtFramework->toQVariant(variant, parent);
	}

	return QVariant(QVariant::Invalid);
}

//==============================================================================
QVariant QtHelpers::toQVariant(const ObjectHandle& object, QObject* parent)
{
	return toQVariant(Variant(object), parent);
}

//==============================================================================
Variant QtHelpers::toVariant(const QVariant& qVariant)
{
	Variant variant;

	auto qtFramework = get<IQtFramework>();

	if (qtFramework != nullptr)
	{
		return qtFramework->toVariant(qVariant);
	}

	return Variant();
}

//==============================================================================
QQuickItem* QtHelpers::findChildByObjectName(QObject* parent, const char* controlName)
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

} // end namespace wgt
