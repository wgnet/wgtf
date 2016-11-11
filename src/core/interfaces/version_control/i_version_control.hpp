//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  i_version_control.h
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef I_VERSION_CONTROL_H_
#define I_VERSION_CONTROL_H_

#pragma once

#include "version_control/i_result.hpp"

namespace wgt
{
class IDepotView;

class IVersionControl
{
public:
	virtual ~IVersionControl(){};

	virtual IResultPtr initialize(const char* depot, const char* port, const char* user, const char* client,
	                              const char* password) = 0;

	virtual std::unique_ptr<IDepotView> createDepotView(const char* depotPath, const char* clientPath) = 0;
};

typedef std::unique_ptr<IVersionControl> IVersionControlPtr;
} // end namespace wgt
#endif // I_VERSION_CONTROL_H_
