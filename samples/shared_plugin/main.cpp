#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QLibraryInfo>
#include <QPluginLoader>
#include <QQmlApplicationEngine>

#include "wg_loader.hpp"

int main(int argc, char *argv[])
{
    QString prefixPath = (QLibraryInfo::location(QLibraryInfo::PrefixPath));
    QDir::setCurrent(prefixPath);

    QStringList paths = QCoreApplication::libraryPaths();
    paths.append(".");
    paths.append("imageformats");
    paths.append("platforms");
    paths.append("sqldrivers");
    QCoreApplication::setLibraryPaths(paths);

    QApplication app(argc, argv);

    QQmlApplicationEngine *engine = new QQmlApplicationEngine;
    engine->addPluginPath(prefixPath);
    engine->addImportPath(prefixPath);
    QDir pluginsDir(prefixPath);
#ifdef QT_DEBUG
    QString pluginName = "qt_sharable_d.dll";
#else
    QString pluginName = "qt_sharable.dll";
#endif
    qDebug() << "loading " << pluginsDir.absoluteFilePath(pluginName);
    QPluginLoader loader(pluginsDir.absoluteFilePath(pluginName));
    QObject *plugin = loader.instance();
    if (plugin)
    {
        WGLoader *loader = qobject_cast<WGLoader*>(plugin);
        if (loader)
        {
            loader->initTF(std::unique_ptr<QQmlEngine>(engine));
        }
        else
        {
            qDebug() << "failed to acquire interface";
        }
    }
    else
    {
        qDebug() << "failed to load plugin";
    }
    engine->load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
