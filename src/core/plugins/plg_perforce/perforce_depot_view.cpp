//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  perforce_depot_view.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "perforce_depot_view.hpp"
#include "perforce_result.hpp"
#include "core_logging/logging.hpp"
#include "core_string_utils/file_path.hpp"

#include <sstream>
#include <regex>

#pragma warning(push)
#pragma warning(disable:4267 4244)
#include "p4/clientApi.h"
#include "p4/spec.h"
#pragma warning(pop)

namespace wgt
{
template<class TCollection>
std::string join(const TCollection& col)
{
	std::stringstream stream;
	for (auto& i : col)
	{
		stream << i << std::endl;
	}
	return stream.str().c_str();
}

class P4ClientUser : public ClientUser
{
public:
	typedef std::vector<std::string> Errors;
	typedef std::vector<std::string> Output;

	void SetInput(const char* input)
	{
		input_ = input;
	}

	const Errors& GetErrors() const { return errors_; }
	const Output& GetOutput() const { return output_; }

	IResultPtr result() const
	{
		auto output = join(GetOutput());
		auto errors = join(GetErrors());
		return IResultPtr(new PerforceResult(output.c_str(), errors.c_str()));
	}

private:
	void OutputError(const char* error) override
	{
		//ClientUser::OutputError(error);
		errors_.emplace_back(error);
	}

	virtual void OutputInfo(char level, const char *data) override
	{
		//ClientUser::OutputInfo(level, data);
		output_.emplace_back(data);
	}

	virtual void OutputText(const char *data, int length) override
	{
		//ClientUser::OutputText(data, length);
		output_.emplace_back(data);
	}

	virtual void InputData(StrBuf *strbuf, Error *e) override
	{
		strbuf->Set(input_.c_str());
	}

	std::string input_;
	Errors errors_;
	Output output_;
};

struct PerforceDepotView::PerforceDepotViewImplementation
{
	PerforceDepotViewImplementation(ClientApiPtr clientApi, const char* depotPath, const char* clientPath)
		: clientApi_(std::move(clientApi)), depotPath_(depotPath), clientPath_(clientPath)
	{
	}

	~PerforceDepotViewImplementation()
	{
		Error e;
		clientApi_->Final(&e);
	}
	ClientApiPtr clientApi_;
	std::string depotPath_;
	std::string clientPath_;
};

PerforceDepotView::PerforceDepotView(ClientApiPtr clientApi, const char* depotPath, const char* clientPath)
	: impl_(new PerforceDepotViewImplementation(std::move(clientApi), depotPath, clientPath))
{
	// Find the matching parent stream specified by the depotPath and replace it with the current depot path

	auto clientInfo = GetClientInfo();
	if(clientInfo.find("clientRoot") == clientInfo.end())
		return;

	auto clientRoot = clientInfo["clientRoot"];

	std::string rootDepotPath = GetRootDepotPath(clientRoot);
	if(rootDepotPath.empty())
		return;

	auto streams = GetStreams();

	// Find the parent stream specified in the depot path
	for(auto& stream : streams)
	{
		if(impl_->depotPath_.compare(0, stream.first.size(), stream.first) == 0)
		{
			impl_->depotPath_.replace(0, stream.first.size(), rootDepotPath);
			break;
		}
	};
}

PerforceDepotView::ResultsInfo PerforceDepotView::GetClientInfo()
{
	// Get the client root from the perforce connection
	std::string cmd("info");
	auto results = RunCommand(cmd);
	if ( !results || results->hasErrors() )
		return ResultsInfo();

	std::string output(results->output());
	if ( output.empty() )
		return ResultsInfo();

	return ParseResults(output);
}

std::string PerforceDepotView::GetRootDepotPath(const std::string& clientRoot)
{
	// Get the depot path from the client root
	std::string cmd = "where " + clientRoot + "/...";
	auto results = RunCommand(cmd);
	if ( !results || results->hasErrors() )
		return "";

	std::string output = results->output();
	if ( output.empty() )
		return "";

	auto paths = ParseResults(output.substr(0, output.find('\n')));
	if ( paths.find("depotFile") == paths.end() )
		return "";

	auto depotPath = paths["depotFile"];
	// Trim the trailing '/...'
	return depotPath.substr(0, depotPath.size() - 4);
}

PerforceDepotView::Streams PerforceDepotView::GetStreams()
{
	std::string cmd = "streams";
	auto results = RunCommand(cmd);
	if ( !results || results->hasErrors() )
		return Streams();

	std::string output = results->output();
	if ( output.empty() )
		return Streams();

	size_t start = 0;
	std::string token = "\n\n";
	auto end = output.find(token);
	Streams streams;
	while ( start != output.length() && end != std::string::npos )
	{
		auto chunk = output.substr(start, end - start);
		if ( !chunk.empty() )
		{
			auto info = ParseResults(chunk);
			if ( info.find("Stream") != info.end() )
			{
				streams[info["Stream"]] = ( std::move(info) );
			}
		}
		start = end + token.size();
		end = output.find("\n\n", start);
	}
	return streams;
}

PerforceDepotView::ResultsInfo PerforceDepotView::ParseResults(const std::string& output) const
{
	ResultsInfo stream;
	// Parse the chunk into the dictionary as key value pairs
	std::regex expression("(^[^ $]+) ?(.*)$");
	auto next = std::sregex_iterator(output.begin(), output.end(), expression);
	std::sregex_iterator end;
	while ( next != end )
	{
		auto match = *next;
		stream[match.str(1)] = match.str(2);
		++next;
	}
	return stream;
}

IResultPtr PerforceDepotView::add(const PathList& filePaths, ChangeListId changeListId)
{
	std::stringstream command;
	if (changeListId != kDefaultChangelist)
	{
		command << "add -c " << changeListId << " -f " << EscapePaths(filePaths);
	}
	else
	{
		command << "add -f " << EscapePaths(filePaths);
	}
	return RunCommand(command.str());
}

IResultPtr PerforceDepotView::remove(const PathList& filePaths, ChangeListId changeListId)
{
	std::stringstream command;
	// The -v flag means that the files do not first have to be on the client
	if (changeListId != kDefaultChangelist)
	{
		command << "delete -c " << changeListId << " -v " << EscapePaths(filePaths);
	}
	else
	{
		command << "delete -v " << EscapePaths(filePaths);
	}
	return RunCommand(command.str());
}

IResultPtr PerforceDepotView::checkout(const PathList& filePaths, ChangeListId changeListId)
{
	std::stringstream command;
	if (changeListId != kDefaultChangelist)
	{
		command << "edit -c " << changeListId << EscapePaths(filePaths);
	}
	else
	{
		command << "edit " << EscapePaths(filePaths);
	}
	return RunCommand(command.str());
}

IResultPtr PerforceDepotView::rename(const FilePairs& filePairs, ChangeListId changeListId)
{
	std::unique_ptr<MultiResult> results(new MultiResult());
	for (auto pair : filePairs)
	{
		results->appendResult(move(pair.first.c_str(), pair.second.c_str(), changeListId));
	}
	return std::move(results);
}

IResultPtr PerforceDepotView::move(const char* srcFile, const char* dstFile, ChangeListId changeListId)
{
	std::stringstream command;
	PathList paths;
	paths.emplace_back(srcFile);
	paths.emplace_back(dstFile);
	getLatest(paths);
	checkout(paths, changeListId);
	if (changeListId != kDefaultChangelist)
	{
		command << "move -c " << changeListId << " -f " << EscapePaths(paths);
	}
	else
	{
		command << "move -f " << EscapePaths(paths);
	}
	return RunCommand(command.str());
}

IResultPtr PerforceDepotView::revert(const PathList& filePaths)
{
	std::string command("revert ");
	command += EscapePaths(filePaths);
	return RunCommand(command);
}

IResultPtr PerforceDepotView::get(const PathList& filePaths, Revision revision)
{
	std::stringstream command;
	if (revision == kHeadRevion)
	{
		command << "sync -f " << EscapePaths(filePaths);
	}
	else
	{
		command << "sync -f " << EscapeRevisionPaths(filePaths, revision);
	}
	return RunCommand(command.str());
}

IResultPtr PerforceDepotView::getLatest(const PathList& filePaths)
{
	return get(filePaths, kHeadRevion);
}

IResultPtr PerforceDepotView::status(const PathList& filePaths)
{
	std::string command("fstat ");
	command += EscapePaths(filePaths);
	return RunCommand(command);
}

IResultPtr PerforceDepotView::submit(const PathList& filePaths, const char* description /*= ""*/, bool bKeepCheckedOut /*= false*/)
{
	ChangeListId changelist;
	auto result = createChangeList(description, changelist);
	if (!result->hasErrors())
	{
		result = reopen(filePaths, changelist);
		if (!result->hasErrors())
		{
			return submit(changelist, bKeepCheckedOut);
		}
	}
	return result;
}

IResultPtr PerforceDepotView::submit(int changelistId, bool bKeepCheckedOut /*= false*/)
{
	std::stringstream command;
	if (bKeepCheckedOut)
	{
		command << "submit -r -c " << changelistId;
	}
	else
	{
		command << "submit -c " << changelistId;
	}
	return RunCommand(command.str());
}

IResultPtr PerforceDepotView::reopen(const PathList& filePaths, ChangeListId changeListId)
{
	std::stringstream command;
	if (changeListId != kDefaultChangelist)
	{
		command << "reopen -c " << changeListId << " " << EscapePaths(filePaths);
	}
	else
	{
		command << "reopen -c default " << EscapePaths(filePaths);
	}
	return RunCommand(command.str());
}

IResultPtr PerforceDepotView::createChangeList(const char* description, ChangeListId& rChangeListId)
{
	std::stringstream input;
	
	input << "Change:	new" << std::endl;
	input << "Client:	" << impl_->clientApi_->GetClient().Text() << std::endl;
	input << "User:	" << impl_->clientApi_->GetUser().Text() << std::endl;
	input << "Status:	new" << std::endl;
	auto formatted = regex_replace(description, std::regex("(\r\n|\r|\n)"), std::string("$1\t"));
	input << "Description:" << formatted << std::endl;
	auto result = RunCommand(std::string("change -i"), input.str());
	if (result->output() && *result->output())
	{
		// Successful output will be something like "Change 23603 created."
		std::string output(result->output());
		auto& start = *std::find(output.begin(), output.end(), ' ');
		rChangeListId = atoi(&start);
	}
	return std::move(result);
}

IResultPtr PerforceDepotView::deleteEmptyChangeList(ChangeListId changeListId)
{
	if (changeListId == kDefaultChangelist)
		return IResultPtr(new PerforceResult("", "Invalid changelist"));

	std::stringstream command;
	command << "change -d " << changeListId;
	return RunCommand(command.str());
}

std::vector<char*> SplitParams(std::string &command)
{
	std::regex paramSplit("[^\\s\"']+|\"[^\"]*\"|'[^']*'");
	std::vector<char*> params;
	std::vector<char*> ends;
	for (auto i = std::cregex_iterator(command.data(), &command.data()[command.size()], paramSplit);
		i != std::cregex_iterator();
		++i)
	{
		for (auto match : *i)
		{
			if (match.length() > 0)
			{
				// Save the end positions to null terminate our strings
				auto end = match.second;
				while (*(end-1) == '"') --end;
				ends.emplace_back(const_cast<char*>(end));
				auto start = match.first;
				while (*start == '"') ++start;
				params.emplace_back(const_cast<char*>(start));
			}
		}
	}
	// Null terminate the strings, must be done after we find our matches
	for (auto& end : ends)
		*end = '\0';

	return std::move(params);
}

const char* PerforceDepotView::getClient() const
{
	return impl_->clientApi_->GetClient().Text();
}

const char* PerforceDepotView::getDepot() const
{
	return impl_->clientApi_->GetPort().Text();
}

const char* PerforceDepotView::getPassword() const
{
	return impl_->clientApi_->GetPassword().Text();
}

const char* PerforceDepotView::getUser() const
{
	return impl_->clientApi_->GetUser().Text();
}

IResultPtr PerforceDepotView::RunCommand(std::string& command, const std::string& input)
{
	auto params = SplitParams(command);
	if (params.size() > 0)
	{
		P4ClientUser ui;
		impl_->clientApi_->SetVar("enableStreams");
		if (params.size() > 1)
			impl_->clientApi_->SetArgv(static_cast<int>(params.size()) - 1, &params[1]);
		else
			impl_->clientApi_->SetArgv(0, nullptr);
		if (!input.empty())
			ui.SetInput(input.c_str());
		impl_->clientApi_->Run(params[0], &ui);
		return ui.result();
	}
	
	return IResultPtr( new PerforceResult("", "Invalid Command") );
}

bool IsLegalChar(char c)
{
	return (c != '@' && c != '#' && c != '%' && c != '*');
}

std::string& ReplaceChars(std::string& depotPath)
{
	// This function takes a path that may contain illegal perforce characters.
	// It replaces the illegal characters with the hexadecimal equivalent, making them legal
	char newSubStr[4] = { 0 };

	for (int i = static_cast<int>(depotPath.size()) - 1; i >= 0; --i)
	{
		auto c = depotPath[i];
		if (!IsLegalChar(c))
		{
			// Replace the illegal char with "%" followed by the hex char value  (example "#" --> "%23")
			sprintf(newSubStr, "%%%2X", static_cast<unsigned char>(c));
			depotPath.replace(i, 1, newSubStr);
		}
	}
	return depotPath;
}

std::string& PerforceDepotView::toPath(std::string& path)
{
	// If no depot path or client path has been specified the path must be fully qualified
	if(impl_->depotPath_.empty() && impl_->clientPath_.empty())
		return path;

	// Is the path already a depot path?
	if(!path.compare(0, impl_->depotPath_.size(), impl_->depotPath_))
		return path;

	// Is the path already a client path?
	if(impl_->clientPath_.size() > 0 && !path.compare(0, impl_->clientPath_.size(), impl_->clientPath_))
		return path;

	// Assume the path is relative to the depot path
	return path = FilePath(impl_->depotPath_, path, FilePath::kDirectorySeparator).str();
}

std::string PerforceDepotView::EscapePaths(const PathList& filePaths)
{
	std::stringstream escapedPaths;
	for (auto path : filePaths)
	{
		escapedPaths << '"' << ReplaceChars(toPath(path)) << '"';
	}
	return escapedPaths.str();
}

std::string PerforceDepotView::EscapeRevisionPaths(const PathList& filePaths, const Revision revision)
{
	std::stringstream escapedPaths;
	for (auto path : filePaths)
	{
		escapedPaths << '"' << ReplaceChars(toPath(path)) << "#" << revision << '"';
	}
	return escapedPaths.str();
}
} // end namespace wgt
