#pragma once

#include "core_reflection/type_class_definition.hpp"
#include "core_reflection/i_definition_manager.hpp"

#include <memory>

#include <QObject>

namespace wgt
{
class QtFrameworkCommon;
class QtModelExtension;

class ModelExtensionManager : public QObject
{
	Q_OBJECT

public:
	ModelExtensionManager(QtFrameworkCommon& qtFrameworkCommon);
	virtual ~ModelExtensionManager();

	void initialise();
	bool registerExtension(const char* id, const char* version, std::function<ManagedObjectPtr()> creator);

	Q_INVOKABLE wgt::QtModelExtension* createExtension(const QString& id, const QString& version, QObject* view);

private:
	struct Implementation;
	friend Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
