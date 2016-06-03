//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  i_application_settings.hpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef I_APPLICATION_SETTINGS_H_
#define I_APPLICATION_SETTINGS_H_

#pragma once

namespace wgt
{
class IApplicationSettings
{
public:
	virtual ~IApplicationSettings(){}

	virtual const char* getString	(const char* name) = 0;
	virtual int			getInt		(const char* name) = 0;
	virtual bool		getBool		(const char* name) = 0;
	virtual float		getFloat	(const char* name) = 0;
	virtual double		getDouble	(const char* name) = 0;

	virtual void		setString	(const char* name, const char* value) = 0;
};
} // end namespace wgt
#endif // I_APPLICATION_SETTINGS_H_
