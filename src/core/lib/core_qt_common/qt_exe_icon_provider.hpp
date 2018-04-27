#pragma once

#if defined( _WIN32 )
#include "interfaces/i_file_icon_provider.hpp"
#include "core_dependency_system/i_interface.hpp"

#include <QQuickImageProvider>
#include <QHash>
#include <QString>
#include <QPixmap>

#include <future>
#include <unordered_map>

namespace wgt
{

/**
 *	Provide the same icon that *.exe files have in Windows Explorer.
 */
class QtExeIconProvider : public Implements<IFileIconProvider>
	, public QQuickImageProvider
{
public:
	QtExeIconProvider();

	virtual std::wstring encodeImage(const std::wstring& filePath) override;
	virtual QPixmap requestPixmap(const QString& id, QSize* size, const QSize& requestedSize) override;

	static const char* providerId();

private:
	std::unordered_map<std::wstring, std::future<QPixmap>> loadingCache_;
	QHash<QString, QPixmap> loadedCache_;
};
} // end namespace wgt
#endif // defined( _WIN32 )
