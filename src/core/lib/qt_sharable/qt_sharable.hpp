#ifndef QT_SHARABLE_H
#define QT_SHARABLE_H

#include <QQmlExtensionPlugin>
#include "core_qt_common/qt_framework_common.hpp"
#include "wg_loader.hpp"

class QtSharable : public QQmlExtensionPlugin, public WGLoader
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.Wargaming.TF")
	Q_INTERFACES(WGLoader)

public:
	void registerTypes(const char* uri) override;

	void initTF(std::unique_ptr<QQmlEngine> qmlEngine) override;

	void finalizeTF() override;

private:
	std::unique_ptr<wgt::QtFrameworkCommon> qtFramework;
};

#endif // QT_SHARABLE_H
