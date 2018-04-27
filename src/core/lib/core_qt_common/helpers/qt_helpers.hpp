#ifndef QT_HELPERS_HPP
#define QT_HELPERS_HPP

#include "../interfaces/i_qt_helpers.hpp"
#include <string>
#include "core_dependency_system/depends.hpp"

namespace wgt
{
class IQtFramework;
class ObjectHandle;
class TypeId;

class QtHelpers : public Implements<IQtHelpers>, Depends<IQtFramework>
{
public:
	virtual ~QtHelpers();

	QVariant toQVariant(const Variant& variant, QObject* parent) override;
	QVariant toQVariant(const ObjectHandle& object, QObject* parent) override;
	Variant toVariant(const QVariant& qVariant) override;

	QQuickItem* findChildByObjectName(QObject* parent, const char* controlName) override;

	template <typename T>
	static std::vector<T*> getChildren(const QObject& parent)
	{
		std::vector<T*> children;
		foreach(auto child, parent.children())
		{
			T* childT = qobject_cast<T*>(child);
			if (childT != nullptr)
			{
				children.push_back(childT);
			}
			auto grandChildren = getChildren<T>(*child);
			children.insert(children.end(), grandChildren.begin(), grandChildren.end());
		}
		return children;
	}
};

} // end namespace wgt
#endif // QT_HELPERS_HPP
