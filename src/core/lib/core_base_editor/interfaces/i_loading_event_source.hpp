//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  i_loading_event_source.hpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef I_LOADING_EVENT_SOURCE_HPP_
#define I_LOADING_EVENT_SOURCE_HPP_

#pragma once
#include "core_common/signal.hpp"

namespace wgt
{
class ILoadingEventSource
{
public:
	virtual ~ILoadingEventSource() {}

	enum class Stage
	{
		Load = 1,
		Initialize = 2,
	};
	
	typedef void OnBeginLoad_t(Stage stage, const std::string& filename);
	typedef void OnEndLoad_t(Stage stage, const std::string& filename);

	typedef std::function<OnBeginLoad_t> OnBeginLoadCallback;
	typedef std::function<OnEndLoad_t> OnEndLoadCallback;

	virtual void beginListening() = 0;
	virtual void endListening() = 0;

	virtual Connection connectBegin(OnBeginLoadCallback callback)
	{
		return Connection();
	}

	virtual Connection connectEnd(OnEndLoadCallback callback)
	{
		return Connection();
	}
};
} // end namespace wgt
#endif // I_RESOURCE_SYSTEM_HPP_
