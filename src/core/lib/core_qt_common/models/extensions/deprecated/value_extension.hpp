#ifndef VALUE_EXTENSION_HPP
#define VALUE_EXTENSION_HPP

#include "i_model_extension_old.hpp"
#include "core_dependency_system/depends.hpp"

#include <QQmlListProperty>
#include <QStringList>

namespace wgt
{
class IQtHelpers;

class ValueExtension : public IModelExtensionOld, Depends<IQtHelpers>
{
	Q_OBJECT

	Q_PROPERTY(QQmlListProperty<QString> roles READ getRoles)

	Q_CLASSINFO("DefaultProperty", "roles")
public:
	ValueExtension();
	virtual ~ValueExtension();

	QHash<int, QByteArray> roleNames() const override;
	QVariant data(const QModelIndex& index, int role) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role) override;

	void onDataAboutToBeChanged(const QModelIndex& index, int role, const QVariant& value) override;
	void onDataChanged(const QModelIndex& index, int role, const QVariant& value) override;

private:
	QQmlListProperty<QString> getRoles() const;

	static void appendRole(QQmlListProperty<QString>* property, QString* value);
	static QString* roleAt(QQmlListProperty<QString>* property, int index);
	static void clearRoles(QQmlListProperty<QString>* property);
	static int countRoles(QQmlListProperty<QString>* property);

	QStringList roles_;
};
} // end namespace wgt
#endif // COMPONENT_EXTENSION_HPP
