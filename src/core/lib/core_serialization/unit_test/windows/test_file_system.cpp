#include "pch.hpp"

#include "core_common/ngt_windows.hpp"
#include "core_serialization/file_system.hpp"

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
	fileSystem_.enumerate(tempPath, [&](IFileInfoPtr&& info){
		infos.emplace_back(std::move(info));
		return true;
	});

	fileSystem_.enumerate(getenv("HOMEDRIVE"), [&](IFileInfoPtr&& info){
		CHECK(fileSystem_.exists(info->fullPath()));
		CHECK(fileSystem_.getFileInfo(info->fullPath())->size() == info->size());
		return true;
	});
}
} // end namespace wgt
