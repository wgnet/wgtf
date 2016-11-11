//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  i_depot_view.h
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef I_DEPOT_VIEW_H_
#define I_DEPOT_VIEW_H_

#pragma once

#include "version_control/i_result.hpp"

#include <vector>
#include <unordered_map>

namespace wgt
{
class IDepotView
{
public:
	typedef std::vector<std::string> PathList;
	typedef std::unordered_map<std::string, std::string> FilePairs;
	typedef int ChangeListId;
	typedef int Revision;
	static const ChangeListId kDefaultChangelist = 0;
	static const Revision kHeadRevion = -1;

	virtual ~IDepotView()
	{
	}
	virtual IResultPtr add(const PathList& filePaths, ChangeListId changeListId = kDefaultChangelist) = 0;
	virtual IResultPtr remove(const PathList& filePaths, ChangeListId changeListId = kDefaultChangelist) = 0;
	virtual IResultPtr checkout(const PathList& filePaths, ChangeListId changeListId = kDefaultChangelist) = 0;
	virtual IResultPtr rename(const FilePairs& filePairs, ChangeListId changeListId = kDefaultChangelist) = 0;
	virtual IResultPtr move(const char* srcFile, const char* dstFile,
	                        ChangeListId changeListId = kDefaultChangelist) = 0;
	virtual IResultPtr revert(const PathList& filePaths) = 0;
	virtual IResultPtr get(const PathList& filePaths, Revision revision = kHeadRevion) = 0;
	virtual IResultPtr getLatest(const PathList& filePaths) = 0;
	virtual IResultPtr status(const PathList& filePaths) = 0;
	virtual IResultPtr submit(const PathList& filePaths, const char* description = "",
	                          bool bKeepCheckedOut = false) = 0;
	virtual IResultPtr submit(int changelistId, bool bKeepCheckedOut = false) = 0;
	virtual IResultPtr reopen(const PathList& filePaths, ChangeListId changeListId = kDefaultChangelist) = 0;
	virtual IResultPtr createChangeList(const char* description, ChangeListId& rChangeListId) = 0;
	virtual IResultPtr deleteEmptyChangeList(ChangeListId changeListId) = 0;

	virtual const char* getClient() const = 0;
	virtual const char* getDepot() const = 0;
	virtual const char* getPassword() const = 0;
	virtual const char* getUser() const = 0;
};

typedef std::unique_ptr<IDepotView> IDepotViewPtr;
typedef std::shared_ptr<IDepotView> IDepotViewSharedPtr;
} // end namespace wgt
#endif // I_DEPOT_VIEW_H_
