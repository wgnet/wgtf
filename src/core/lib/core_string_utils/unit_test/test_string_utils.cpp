#include "pch.hpp"

#include "core_unit_test/unit_test.hpp"

#include "core_string_utils/file_path.hpp"
#include "core_string_utils/string_utils.hpp"

namespace wgt
{
const char FilePath::kNativeDirectorySeparator;
const char FilePath::kNativeAltDirectorySeparator;
const char FilePath::kAltDirectorySeparator;
const char FilePath::kDirectorySeparator;

TEST( combineCleanPaths )
{
	auto part1 = "test";
	auto part2 = "path";
	auto filePart = "file.ext";
	auto str1 = std::string(part1);
	auto str2 = std::string(part2);

	// Check const char*
	CHECK(FilePath::combine(part1, "") == str1 + FilePath::kNativeDirectorySeparator);
	CHECK(FilePath::combine("", part2) == str2);
	CHECK(FilePath::combine(part1, "/") == str1 + FilePath::kNativeDirectorySeparator);
	CHECK(FilePath::combine(part1, "\\") == str1 + FilePath::kNativeDirectorySeparator);
	// Check string
	CHECK(FilePath::combine(str1, str2) == str1 + FilePath::kNativeDirectorySeparator + str2);

	// File extension check
	auto path = FilePath::combine(FilePath::combine(part1, part2), filePart);
	CHECK(path == str1 + FilePath::kNativeDirectorySeparator + part2 + FilePath::kNativeDirectorySeparator + filePart);
	CHECK(FilePath::combine("", path) == path);
	CHECK(FilePath::combine(path, "") == path);
}


TEST( combineDirtyPaths )
{
	auto str1 = std::string("test");
	auto str2 = std::string("path");
	const char dirSep[2] = {FilePath::kDirectorySeparator, '\0'};
	const char altDirSep[2] = {FilePath::kAltDirectorySeparator, '\0'};
	auto sep1 = std::string(dirSep);
	auto sep2 = std::string(altDirSep);

	// Paths have separators and separator is specified
	CHECK(FilePath::combine(str1 + '\\', str2, FilePath::kDirectorySeparator) == str1 + FilePath::kDirectorySeparator + str2);
	CHECK(FilePath::combine(str1 + '/', str2, FilePath::kDirectorySeparator) == str1 + FilePath::kDirectorySeparator + str2);
	CHECK(FilePath::combine(str1 + '\\', str2, FilePath::kAltDirectorySeparator) == str1 + FilePath::kAltDirectorySeparator + str2);
	CHECK(FilePath::combine(str1 + '/', str2, FilePath::kAltDirectorySeparator) == str1 + FilePath::kAltDirectorySeparator + str2);
	CHECK(FilePath::combine(str1, sep1 + str2, FilePath::kDirectorySeparator) == str1 + FilePath::kDirectorySeparator + str2);
	CHECK(FilePath::combine(str1, sep2 + str2, FilePath::kDirectorySeparator) == str1 + FilePath::kDirectorySeparator + str2);

	CHECK(FilePath::combine(str1 + '\\', sep1 + str2) == str1 + FilePath::kNativeDirectorySeparator + str2);
	CHECK(FilePath::combine(str1 + '\\', sep2 + str2) == str1 + FilePath::kNativeDirectorySeparator + str2);
	CHECK(FilePath::combine(str1 + '/', sep1 + str2) == str1 + FilePath::kNativeDirectorySeparator + str2);
	CHECK(FilePath::combine(str1 + '/', sep2 + str2) == str1 + FilePath::kNativeDirectorySeparator + str2);
}

TEST( filePathMembers )
{
	auto part1 = std::string("test");
	auto part2 = std::string("path");
	auto file = std::string("file");
	auto ext = std::string("ext");

	FilePath path = FilePath::combine(part1, part2);
	CHECK(path.getFolder() == part1 + FilePath::kNativeDirectorySeparator);
	CHECK(path.getFileNoExtension() == part2);
	CHECK(path.getFileWithExtension() == part2);
	CHECK(path.getExtension() == "");

	FilePath dirPath = FilePath::combine(path.str(), "/");
	CHECK(dirPath.getFolder() == part1 + FilePath::kNativeDirectorySeparator + part2 + FilePath::kNativeDirectorySeparator);
	CHECK(dirPath.getFileNoExtension() == "");
	CHECK(dirPath.getFileWithExtension() == "");
	CHECK(dirPath.getExtension() == "");

	FilePath fullPath = FilePath::combine(path.str(), file + FilePath::kExtensionSeparator + ext);
	CHECK(fullPath.getFolder() == part1 + FilePath::kNativeDirectorySeparator + part2 + FilePath::kNativeDirectorySeparator);
	CHECK(fullPath.getFileNoExtension() == file);
	CHECK(fullPath.getFileWithExtension() == file + FilePath::kExtensionSeparator + ext);
	CHECK(fullPath.getExtension() == ext);

	const std::string dirWithSeperator(part1 + FilePath::kNativeDirectorySeparator);
	const std::string dirWithAltSeperator(part1 + FilePath::kNativeAltDirectorySeparator);
	const std::string fileWithExtension(file + FilePath::kExtensionSeparator + ext);
	const std::string fullPathWithSeperator(dirWithSeperator + fileWithExtension);
	const std::string fullPathWithAltSeperator(dirWithAltSeperator + fileWithExtension);

	CHECK(FilePath::getFolder(fullPathWithSeperator) == dirWithSeperator);
	CHECK(FilePath::getFolder(fullPathWithAltSeperator) == dirWithAltSeperator);
	CHECK(FilePath::getFileNoExtension(fullPathWithSeperator) == file);
	CHECK(FilePath::getFileNoExtension(fullPathWithAltSeperator) == file);
	CHECK(FilePath::getFileWithExtension(fullPathWithSeperator) == fileWithExtension);
	CHECK(FilePath::getFileWithExtension(fullPathWithAltSeperator) == fileWithExtension);
}

TEST(convertString)
{
	const std::string input("helloworld");
	std::wstring wstr = StringUtils::to_wstring(input);
	std::string str = StringUtils::to_string(wstr);
	CHECK(input == str);
}

TEST(eraseString)
{
	const std::string str("helloworld");

	std::string input = str;
	StringUtils::erase_string(input, "unknown");
	CHECK(input == str);

	StringUtils::erase_string(input, "world");
	CHECK(input == "hello");
}
} // end namespace wgt
