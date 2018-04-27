//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  perforce_depot_view.h
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef PERFORCE_DEPOT_VIEW_H_
#define PERFORCE_DEPOT_VIEW_H_

#pragma once

#include "version_control/i_depot_view.hpp"

class ClientApi;

namespace wgt
{
class IVersionControl;

class PerforceDepotView : public IDepotView
{
	typedef std::unique_ptr<ClientApi> ClientApiPtr;
	typedef std::unordered_map<std::string, std::string> ResultsInfo;
	typedef std::unordered_map<std::string, ResultsInfo> Streams;

public:
	PerforceDepotView(ClientApiPtr clientApi, const char* depotPath, const char* clientPath);

private:
	virtual IResultPtr add(const PathList& filePaths, ChangeListId changeListId) override;
	virtual IResultPtr remove(const PathList& filePaths, ChangeListId changeListId) override;
	virtual IResultPtr checkout(const PathList& filePaths, ChangeListId changeListId) override;
	virtual IResultPtr rename(const FilePairs& filePairs, ChangeListId changeListId) override;
	virtual IResultPtr move(const char* srcFile, const char* dstFile, ChangeListId changeListId) override;
	virtual IResultPtr revert(const PathList& filePaths) override;
	virtual IResultPtr revertUnchanged(const PathList& filePaths) override;
	virtual IResultPtr get(const PathList& filePaths, Revision revision) override;
	virtual IResultPtr getRevisionBetween(const PathList& filePaths, int fromChangelist, int toChangelist) override;
	virtual IResultPtr getLatest(const PathList& filePaths) override;
	virtual IResultPtr status(const PathList& filePaths) override;
	virtual IResultPtr submit(const PathList& filePaths, const char* description = "",
	                          bool bKeepCheckedOut = false) override;
	virtual IResultPtr submit(int changelistId, bool bKeepCheckedOut = false) override;
	virtual IResultPtr reopen(const PathList& filePaths, ChangeListId changeListId) override;
	virtual IResultPtr createChangeList(const char* description, ChangeListId& rChangeListId) override;
	virtual IResultPtr deleteEmptyChangeList(ChangeListId changeListId) override;
	virtual IResultPtr querySubDirs(const char* parentDir = "") override;
	virtual IResultPtr getTicket() override;

	virtual const char* getClient() const override;
	virtual const char* getDepot() const override;
	virtual const char* getPassword() const override;
	virtual const char* getUser() const override;
	virtual std::string getClientRoot() override;
	virtual std::string getDepotRoot() override;

	virtual std::vector<std::string> getClientNames() override;

	IResultPtr RunCommand(std::string& command, const std::string& input = std::string());

	std::string& toPath(std::string& path);
	std::string& toDepotDirQueryPath(std::string& path);

	std::string EscapePaths(const PathList& filePaths);
	std::string EscapeDepotDirQueryPaths(const PathList& filePaths);
	std::string EscapeRevisionPaths(const PathList& filePaths, const Revision revision);
	std::string EscapePathsBetween(const PathList& cleanPaths, int fromChange, int toChange);
	ResultsInfo ParseResults(const std::string& output) const;

	ResultsInfo GetClientInfo();
	std::string GetRootDepotPath(const std::string& clientRoot);
	Streams GetStreams();

	struct PerforceDepotViewImplementation;
	std::unique_ptr<PerforceDepotViewImplementation> impl_;
};
} // end namespace wgt
#endif // PERFORCE_DEPOT_VIEW_H_
