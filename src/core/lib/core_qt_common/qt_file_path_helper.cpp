#include "qt_file_path_helper.hpp"
#include "core_string_utils/file_path.hpp"
#include "core_string_utils/string_utils.hpp"
#include "core_common/platform_path.hpp"
#include "core_serialization/i_file_system.hpp"

namespace wgt
{

QtFilePathHelper::QtFilePathHelper()
{
	wchar_t path[MAX_PATH];
	::GetModuleFileNameW(NULL, path, MAX_PATH);

	const std::wstring pathWStr(path);
	std::string pathStr = FilePath::getFolder(
		std::string(pathWStr.begin(), pathWStr.end())).c_str();
	if(pathStr.back() == FilePath::kDirectorySeparator ||
	   pathStr.back() == FilePath::kAltDirectorySeparator)
	{
		pathStr.pop_back();
	}

	applicationFolder_ = pathStr.c_str();
}

bool QtFilePathHelper::fileExists(const QString& file) const
{
	if(QFile(file).exists())
	{
		return true;
	}
	if (auto fileSystem = get<IFileSystem>())
	{
		return fileSystem->exists(file.toUtf8().constData());
	}
	return false;
}

bool QtFilePathHelper::filesExist(QString files) const
{
	files.replace('\r', '\n');
	auto allFiles = files.split('\n', QString::SkipEmptyParts);
	return !allFiles.empty() && std::all_of(allFiles.begin(), allFiles.end(), [this](const auto& file)
	{
		return fileExists(file);
	});
}

bool QtFilePathHelper::folderExists(const QString& folder) const
{
	return QDir(folder).exists();
}

bool QtFilePathHelper::createFolder(const QString& folder) const
{
	QDir dir(folder);
	return dir.exists() || dir.mkpath(".");
}

QStringList QtFilePathHelper::getFilesInFolder(const QString& folder, const QStringList& filters) const
{
	QStringList files;
	const QDir dir(folder);
	if(dir.exists())
	{
		return dir.entryList(filters);
	}
	return files;
}

std::string QtFilePathHelper::removeQRCPrefix(const char* path)
{
	std::string filepath(path);
	StringUtils::erase_string(filepath, "qrc:");
	StringUtils::erase_string(filepath, ":");

	if (!filepath.empty())
	{
		int index = 0;
		while (filepath[index] == FilePath::kDirectorySeparator || filepath[index] == FilePath::kAltDirectorySeparator)
		{
			++index;
		}

		if (index > 0)
		{
			filepath.erase(0, index);
		}
	}

	return filepath;
}

} // end namespace wgt