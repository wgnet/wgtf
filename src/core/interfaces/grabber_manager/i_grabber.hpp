#pragma once

#include <string>
#include <vector>
#include "core_ui_framework/input_handler.hpp"

namespace wgt
{
class ObjectHandle;
class IDefinitionManager;
typedef std::string EnvironmentId;

struct IGrabber : public InputHandler
{
	virtual ~IGrabber() {}

	virtual ObjectHandle getPropertiesObject() = 0;
	virtual const ObjectHandle getPropertiesObject() const = 0;
	virtual const char* getTitle() const = 0;
	virtual const char* getId() const = 0;
	virtual const std::vector<std::string>& getUniquePropertyNames() const = 0;
	virtual bool isSupportedEnvironment(const EnvironmentId& id) const = 0;

	virtual void draw() {}
	virtual void postDraw() {}
	virtual void update() {}

	virtual void onPrePreferencesSaved() {}
	virtual void onPostPreferencesLoaded() {}
	virtual void onPreEnvironmentChanged(const EnvironmentId& id) {}
	virtual void onPostEnvironmentChanged(const EnvironmentId& id) {}
	virtual void onPostSettingsChanged() {}
};
} // end namespace wgt
