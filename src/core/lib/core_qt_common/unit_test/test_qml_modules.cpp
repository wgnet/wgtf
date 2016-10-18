#include "pch.hpp"

#include "core_unit_test/unit_test.hpp"
#include "core_common/platform_path.hpp"
#include "core_common/platform_dll.hpp"
#include "core_string_utils/file_path.hpp"
#include "core_string_utils/string_utils.hpp"
#include <array>
#include <fstream>
#include <QDir>
#include <QStringList>

namespace wgt
{
bool isExcludedFile(const QString& file)
{
	return file.startsWith("XDep") || file.startsWith("__dummy");
}

std::string getFileText(const QString& path, std::string& errors)
{
	std::string str;
	const std::string filePath(path.toUtf8().constData());

	std::ifstream file(filePath, std::ios::in | std::ios::ate);
	if (!file.is_open())
	{
		errors += filePath + " could not be opened\n";
	}
	else
	{
		const int size = static_cast<int>(file.tellg());
		file.seekg(0, std::ios::beg);
		str.resize(size);
		file.read(&str[0], str.size());
		file.close();

		if (str.empty())
		{
			errors += filePath + " is empty\n";
		}
	}

	return str;
}

void checkValidComponent(const QString& path, std::string& errors)
{
	const std::string filePath(path.toUtf8().constData());
	const std::string str = getFileText(path, errors);

	const auto index = str.find("WGComponent");
	if (index == std::string::npos)
	{
		errors += filePath + " did not include WGComponent\n";
		return;
	}

	if (str.find("\"" + FilePath::getFileNoExtension(filePath) + "\"", index) == std::string::npos)
	{
		errors += filePath + " WGComponent was not named after file\n";
		return;
	}
}

/**
* Tests all files in core/plugin include the WGComponent type
*/
TEST(testQmlComponentTypes)
{
	const QString srcPath("./Plugins/QML");
	const QDir srcDir(srcPath);
	CHECK(srcDir.exists());

	std::string errors;

	const auto files = srcDir.entryList(QStringList("*.qml"));
	for (const auto& file : files)
	{
		if (!isExcludedFile(file))
		{
			const QString component(srcPath + FilePath::kNativeDirectorySeparator + file);
			checkValidComponent(component, errors);
		}
	}

	CHECK(files.count() > 0);
	if (!errors.empty())
	{
		FAIL(errors.c_str());
	}
}
} // end namespace wgt
