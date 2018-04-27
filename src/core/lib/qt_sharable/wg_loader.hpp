#pragma once

#include <memory>

#include <QObject>
#include <QQmlEngine>

class WGLoader
{
public:
	virtual ~WGLoader()
	{
	}

	virtual void initTF(std::unique_ptr<QQmlEngine> qmlEngine) = 0;

	virtual void finalizeTF() = 0;
};

#define WGLoader_iid "org.Wargaming.WG_LOADER"

Q_DECLARE_INTERFACE(WGLoader, WGLoader_iid)
