//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  perforce_version_control.h
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef PERFORCE_VERSION_CONTROL_H_
#define PERFORCE_VERSION_CONTROL_H_

#pragma once

#include "core_dependency_system/i_interface.hpp"
#include "version_control/i_version_control.hpp"

namespace wgt
{
class PerforceVersionControl : public Implements<IVersionControl>
{
public:
	PerforceVersionControl();

private:
	virtual IResultPtr initialize(const char* depot, const char* port, const char* user, const char* client,
	                              const char* password) override;

	virtual std::unique_ptr<IDepotView> createDepotView(const char* depotPath, const char* clientPath) override;

	struct PerforceVersionControlImplementation;
	std::unique_ptr<PerforceVersionControlImplementation> impl_;
};
} // end namespace wgt
#endif // PERFORCE_VERSION_CONTROL_H_
