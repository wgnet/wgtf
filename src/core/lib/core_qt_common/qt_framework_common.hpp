#pragma once

#include <memory>
#include <atomic>
#include <vector>

class QQmlIncubationController;
class QQmlEngine;
class QString;
class QUrl;
class QVariant;
class QObject;

namespace wgt
{
class QtScriptingEngineBase;
class QtDefaultSpacing;
class QtGlobalSettings;
class QmlComponentManager;
class QmlComponent;
class QtPalette;
class IComponentProvider;
class IComponent;
class ModelExtensionManager;
class IQtTypeConverter;
class Variant;
class IDefinitionManager;

/**
* Implementation for context manager independent part of QtFramework.
* This class is not intended to be used as a base class.
*/
class QtFrameworkCommon
{
public:
	QtFrameworkCommon(std::unique_ptr<QQmlEngine> qmlEngine, std::unique_ptr<QtScriptingEngineBase> scriptingEngine,
	                  IDefinitionManager* definitionManager = nullptr);
	virtual ~QtFrameworkCommon();

	QmlComponentManager* qmlComponentManager();
	const QmlComponentManager* qmlComponentManager() const;

	ModelExtensionManager* modelExtensionManager();
	const ModelExtensionManager* modelExtensionManager() const;

	QQmlEngine* qmlEngine() const;
	QtScriptingEngineBase* scriptingEngine();

	QtPalette* palette() const;
	QtGlobalSettings* qtGlobalSettings() const;

	IDefinitionManager* definitionManager() const;

	void setIncubationTime(int msecs);
	int incubationTime() const;

	QString resolveFilePath(const char* relativePath) const;
	QUrl resolveQmlPath(const char* relativePath) const;

	static QString resolveFilePath(const QQmlEngine& qmlEngine, const char* relativePath);
	static QUrl resolveQmlPath(const QQmlEngine& qmlEngine, const char* relativePath);

	void initialise();
	void finalise();

	QmlComponent* createComponent(const QUrl& resource);

	void registerTypeConverter(IQtTypeConverter& converter);
	void deregisterTypeConverter(IQtTypeConverter& converter);

	QVariant toQVariant(const Variant& variant, QObject* parent) const;
	Variant toVariant(const QVariant& qVariant) const;

private:
	struct Implementation;
	friend Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
