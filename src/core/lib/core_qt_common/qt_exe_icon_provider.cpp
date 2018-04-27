#include "qt_exe_icon_provider.hpp"

#if defined( _WIN32 )
#include "core_common/assert.hpp"
#include "core_common/ngt_windows.hpp"
#include "core_string_utils/file_path.hpp"
#include "core_string_utils/string_utils.hpp"

#include <QtWin>

namespace wgt
{

namespace QtExeIconProviderDetails
{

std::wstring toUrl(const char* providerId, const std::wstring& imagePath)
{
	return L"image://" +
		StringUtils::to_wstring(QtExeIconProvider::providerId()) +
		L"/" +
		imagePath;
}

QPixmap loadIcon(const std::wstring& executablePath)
{
	// https://msdn.microsoft.com/en-us/library/ms997538.aspx
	// "You must initialize Component Object Model (COM) with CoInitialize
	// "or OleInitialize prior to calling SHGetFileInfo."
	const auto initResult = CoInitialize(NULL /* reserved */);

	// Load a "Large" icon image, which is usually 32x32
	SHFILEINFO shellFileInfo {};
	auto pszPath = static_cast<LPWSTR>(const_cast<wchar_t*>(executablePath.c_str()));
	const auto hr = SHGetFileInfo(
		pszPath,
		0 /* dwFileAttributes */,
		&shellFileInfo,
		sizeof(SHFILEINFO), 
		SHGFI_ICON | SHGFI_LARGEICON /*| SHGFI_LINKOVERLAY*/);
	TF_ASSERT(SUCCEEDED(hr));

	const auto result = QtWin::fromHICON(shellFileInfo.hIcon);

	// https://msdn.microsoft.com/en-us/library/ms997538.aspx
	// "If SHGetFileInfo returns an icon handle in the hIcon member of the
	// SHFILEINFO structure pointed to by psfi, you are responsible for
	// freeing it with DestroyIcon when you no longer need it."
	DestroyIcon(shellFileInfo.hIcon);

	// https://msdn.microsoft.com/en-us/library/windows/desktop/ms678543(v=vs.85).aspx
	// "each successful call to CoInitialize or CoInitializeEx,
	// including those that return S_FALSE,
	// must be balanced by a corresponding call to CoUninitialize."
	CoUninitialize();

	return result;
}

} // namespace QtExeIconProviderDetails

QtExeIconProvider::QtExeIconProvider()
	: QQuickImageProvider(ImageType::Pixmap)
{
}

std::wstring QtExeIconProvider::encodeImage(
	const std::wstring& executablePath) /* override */
{
	if (executablePath.empty())
	{
		return L"";
	}

	const FilePath filePath(StringUtils::to_string(executablePath));
	const auto imagePath = StringUtils::to_wstring(filePath.getFileNoExtension());
	if (loadingCache_.find(imagePath) != loadingCache_.end())
	{
		return QtExeIconProviderDetails::toUrl(QtExeIconProvider::providerId(), imagePath);
	}

	const auto qImagePath = QString::fromStdWString(imagePath);
	if (loadedCache_.find(qImagePath) != loadedCache_.end())
	{
		return QtExeIconProviderDetails::toUrl(QtExeIconProvider::providerId(), imagePath);
	}

	// Note: the Microsoft documentation for SHGetFileInfo says to call it from
	// a background thread. But calling it on a background thread results in
	// random failures. Leaving it running in the foreground for now.
	// https://msdn.microsoft.com/en-us/library/ms997538.aspx
	// "You should call this function from a background thread.
	// Failure to do so could cause the UI to stop responding."
	const bool loadAsync = false;
	if (loadAsync)
	{
		loadingCache_[imagePath] = std::async([executablePath]() -> QPixmap
		{
			return QtExeIconProviderDetails::loadIcon(executablePath);
		});
	}
	else
	{
		loadedCache_[qImagePath] = QtExeIconProviderDetails::loadIcon(executablePath);
	}

	return QtExeIconProviderDetails::toUrl(QtExeIconProvider::providerId(), imagePath);
}

QPixmap QtExeIconProvider::requestPixmap(const QString& id, QSize* size, const QSize& requestedSize)
{
	{
		const auto loadedIt = loadedCache_.find(id);
		if (loadedIt != loadedCache_.cend())
		{
			if (requestedSize.isValid())
			{
				return loadedIt.value().scaled(requestedSize);
			}
			return loadedIt.value();
		}
	}

	{
		const auto loadingIt = loadingCache_.find(id.toStdWString());
		if (loadingIt != loadingCache_.cend())
		{
			const auto loadedIt = loadedCache_.insert(id, loadingIt->second.get());
			loadingCache_.erase(loadingIt);
			if (requestedSize.isValid())
			{
				return loadedIt.value().scaled(requestedSize);
			}
			return loadedIt.value();
		}
	}

	// Returning an empty pixmap causes Qt to print a warning message
	return QPixmap();
}

const char* QtExeIconProvider::providerId()
{
	return "QtExeIconProvider";
}

} // end namespace wgt
#endif // defined( _WIN32 )
