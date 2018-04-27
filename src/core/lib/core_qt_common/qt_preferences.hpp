#ifndef QT_PREFERENCES_HPP
#define QT_PREFERENCES_HPP

#include "core_ui_framework/i_preferences.hpp"
#include "core_dependency_system/i_interface.hpp"

namespace wgt
{
class IComponentContext;

class QtPreferences : public Implements<IPreferences>
{
public:
	QtPreferences();
	~QtPreferences();

	virtual GenericObjectPtr getPreference(const std::string& key, const std::string& preferenceKey = "") override;
	virtual bool preferenceExists (const std::string& key, const std::string& preferenceKey = "") const override;
	virtual void registerPreferencesListener(std::shared_ptr<IPreferencesListener>& listener) override;
	virtual void deregisterPreferencesListener(std::shared_ptr<IPreferencesListener>& listener) override;
	virtual bool writePreferenceToFile(const char* fileName, bool notifyListeners) override;
	virtual bool loadPreferenceFromFile(const char* fileName, bool notifyListeners) override;
	virtual bool hasPreferencesFile(const char* fileName) const override;
	virtual void loadDefaultPreferences() override;
	virtual bool setPreferencesFolder(const char* directory) override;
	virtual bool setPreferencesFolderAsUserDirectory() override;
	virtual const char* getPreferencesExtension() const override;
	virtual void setDefaultPreferenceKey(const std::string& preferenceKey) override;
	virtual const std::string& getDefaultPreferenceKey() const override;
	virtual void clearPreferences() override;

private:
	class Implementation;
	std::unique_ptr<Implementation> pImpl_;
};
} // end namespace wgt
#endif // QT_PREFERENCES_HPP
