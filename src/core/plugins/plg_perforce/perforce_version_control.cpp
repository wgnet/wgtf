//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  perforce_version_control.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "perforce_version_control.hpp"
#include "perforce_result.hpp"
#include "perforce_depot_view.hpp"

#pragma warning(push)
#pragma warning(disable:4267 4244)
#include "p4/clientApi.h"
#pragma warning(pop)

namespace wgt
{
struct PerforceVersionControl::PerforceVersionControlImplementation
{
	PerforceVersionControlImplementation( PerforceVersionControl& self )
		: self(self)
	{
	}

	PerforceVersionControl& self;
	std::string depot;
	std::string port;
	std::string user;
	std::string client;
	std::string password;
};


PerforceVersionControl::PerforceVersionControl()
	: impl_(new PerforceVersionControlImplementation(*this))
{
}

IResultPtr PerforceVersionControl::initialize(const char* depot, const char* port, const char* user, const char* client, const char * password)
{
	impl_->depot = depot;
	impl_->port = port;
	impl_->user = user;
	impl_->client = client;
	impl_->password = password;
	return IResultPtr( new PerforceResult("", "") );
}

std::unique_ptr<IDepotView> PerforceVersionControl::createDepotView(const char* depotPath, const char* clientPath)
{
	std::unique_ptr<ClientApi> clientApi(new ClientApi());

	clientApi->SetPort(impl_->port.c_str());
	clientApi->SetUser(impl_->user.c_str());
	clientApi->SetClient(impl_->client.c_str());
	clientApi->SetPassword(impl_->password.c_str());

	clientApi->SetProtocol("api", "76"); // 2014.1 compatible server
	clientApi->SetProtocol("tag", "");			// Enabled Tagged output
	clientApi->SetProtocol("specstring", "");	// Parse Forms

	Error e;
	clientApi->Init(&e);

	StrBuf m;
	e.Fmt(&m);
	if (!e.Test())
	{
		clientApi->SetProg("NGT perforce plugin");
		return IDepotViewPtr(new PerforceDepotView(std::move(clientApi), depotPath, clientPath));
	}

	return IDepotViewPtr();
}
} // end namespace wgt
