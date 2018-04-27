#include "pch.hpp"

#include "core_common/ngt_windows.hpp"
#include "core_serialization/file_system.hpp"
#include <direct.h>

namespace wgt
{
TEST(file_sytem)
{
	FileSystem fileSystem_;

	char tempPath[MAX_PATH] = { 0 };
	::GetTempPathA(MAX_PATH, tempPath);
	CHECK(fileSystem_.exists(tempPath));
	CHECK(fileSystem_.getFileInfo(tempPath)->isDirectory());
	CHECK(fileSystem_.getFileType(tempPath) == IFileSystem::Directory);

	char filename[MAX_PATH] = { 0 };
	char filename2[MAX_PATH] = { 0 };
	::GetTempFileNameA(tempPath, "", 0, filename);
	::GetTempFileNameA(tempPath, "", 0, filename2);
	CHECK(fileSystem_.exists(filename));
	CHECK(fileSystem_.exists(filename2));
	CHECK(fileSystem_.remove(filename));
	CHECK(fileSystem_.remove(filename2));
	CHECK(!fileSystem_.exists(filename));
	CHECK(!fileSystem_.exists(filename2));

	auto data = "TestString";
	auto datalen = strlen(data);
	fileSystem_.writeFile(filename, data, datalen, std::ios::out);
	CHECK(fileSystem_.exists(filename));
	CHECK(fileSystem_.getFileType(filename) == IFileSystem::File);
	{
		auto stream = fileSystem_.readFile(filename, std::ios::in);
		char buffer[MAX_PATH] = { 0 };
		stream->readRaw(&buffer, datalen);
		CHECK(strcmp(data, buffer) == 0);
	}

	CHECK(fileSystem_.copy(filename, filename2));
	CHECK(fileSystem_.exists(filename2));
	{
		auto stream = fileSystem_.readFile(filename2, std::ios::in);
		char buffer[MAX_PATH] = { 0 };
		stream->readRaw(&buffer, datalen);
		CHECK(strcmp(data, buffer) == 0);
	}

	CHECK(fileSystem_.remove(filename2));
	CHECK(!fileSystem_.exists(filename2));

	CHECK(fileSystem_.move(filename, filename2));
	CHECK(!fileSystem_.exists(filename));
	CHECK(fileSystem_.exists(filename2));
	{
		auto stream = fileSystem_.readFile(filename2, std::ios::in);
		char buffer[MAX_PATH] = { 0 };
		stream->readRaw(&buffer, datalen);
		CHECK(strcmp(data, buffer) == 0);
	}

	CHECK(fileSystem_.remove(filename2));
	CHECK(!fileSystem_.exists(filename2));

	std::vector<IFileInfoPtr> infos;
	fileSystem_.enumerate(tempPath, [&](IFileInfoPtr&& info) {
		infos.emplace_back(std::move(info));
		return true;
	});

	fileSystem_.enumerate(getenv("HOMEDRIVE"), [&](IFileInfoPtr&& info) {
		CHECK(fileSystem_.exists(info->fullPath()->c_str()));
		CHECK(fileSystem_.getFileInfo(info->fullPath()->c_str())->size() == info->size());
		return true;
	});

	char userDirectoryPath[MAX_PATH];
	CHECK(GetUserDirectoryPath(userDirectoryPath));
	CHECK(fileSystem_.exists(userDirectoryPath));

	const std::string userToolsPath = std::string(userDirectoryPath) +
		FilePath::kNativeDirectorySeparator +
		"wgtools_testing";

	const std::string userFilenamePath = userToolsPath +
		FilePath::kNativeDirectorySeparator +
		"wgtools_testing.txt";

	// clean up any stray files from a previous test run
	if (fileSystem_.exists(userToolsPath.c_str()))
	{
		if (fileSystem_.exists(userFilenamePath.c_str()))
		{
			CHECK(fileSystem_.remove(userFilenamePath.c_str()));
		}
		CHECK(fileSystem_.removeDirectory(userToolsPath.c_str()));
	}

	// test directory creation and destruction and a file within that directory
	CHECK(!fileSystem_.exists(userToolsPath.c_str()));
	CHECK(!fileSystem_.exists(userFilenamePath.c_str()));

	CHECK(fileSystem_.createDirectory(userToolsPath.c_str()));
	CHECK(fileSystem_.exists(userToolsPath.c_str()));
	CHECK(fileSystem_.createDirectory(userToolsPath.c_str())); // Ensure multiple calls return true
	CHECK(fileSystem_.removeDirectory(userToolsPath.c_str()));
	
	{ // check file operations inside the test directory
		CHECK(!fileSystem_.exists(userToolsPath.c_str()));
		CHECK(fileSystem_.createDirectory(userToolsPath.c_str()));
		CHECK(fileSystem_.exists(userToolsPath.c_str()));

		const std::string testFileContents = "wgtools_testing.txt file contents";
		const size_t dataSize = strlen(testFileContents.c_str());
		CHECK(fileSystem_.writeFile(userFilenamePath.c_str(), testFileContents.c_str(), dataSize, std::ios::out));
		CHECK(fileSystem_.exists(userFilenamePath.c_str()));
		CHECK(fileSystem_.remove(userFilenamePath.c_str()));
		CHECK(!fileSystem_.exists(userFilenamePath.c_str()));
		CHECK(fileSystem_.writeFile(userFilenamePath.c_str(), testFileContents.c_str(), dataSize, std::ios::out));
		CHECK(fileSystem_.exists(userFilenamePath.c_str()));

		auto fileInfo = fileSystem_.getFileInfo(userFilenamePath.c_str());
		CHECK(dataSize == fileInfo->size());
		{
			auto stream = fileSystem_.readFile(userFilenamePath.c_str(), std::ios::in);
			const size_t bufferSize = 256;
			char buffer[bufferSize] = { 0 };
			CHECK(dataSize < bufferSize);
			stream->readRaw(&buffer, dataSize);
			CHECK(strcmp(testFileContents.c_str(), buffer) == 0);
		}

		// if a stream has been opened to read from the file, the stream must be closed
		// before this point. Destroy the file first as the directory can't be destroyed
		// until this is done

		CHECK(fileSystem_.exists(userToolsPath.c_str()));
		CHECK(!fileSystem_.removeDirectory(userToolsPath.c_str()));
		CHECK(fileSystem_.exists(userToolsPath.c_str()));

		CHECK(fileSystem_.exists(userFilenamePath.c_str()));
		CHECK(fileSystem_.remove(userFilenamePath.c_str()));
		CHECK(!fileSystem_.exists(userFilenamePath.c_str()));

		CHECK(fileSystem_.exists(userToolsPath.c_str()));
		CHECK(fileSystem_.removeDirectory(userToolsPath.c_str()));
		CHECK(!fileSystem_.exists(userToolsPath.c_str()));
	}
}
} // end namespace wgt
