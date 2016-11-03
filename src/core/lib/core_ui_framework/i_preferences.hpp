#ifndef I_PREFERENCE_HPP
#define I_PREFERENCE_HPP

#include "core_reflection/generic/generic_object.hpp"
#include "core_reflection/mutable_vector.hpp"

namespace wgt
{
class IPreferencesListener
{
public:
    virtual void prePreferencesChanged() {}
    virtual void postPreferencesChanged() {}
    virtual void prePreferenceSaved() {}
};

class IPreferences
{
public:
	virtual ~IPreferences() {}

    typedef MutableVector< std::weak_ptr< IPreferencesListener > > PreferencesListeners;

	virtual GenericObjectPtr & getPreference( const char * key ) = 0;
    virtual bool preferenceExists( const char* key ) const = 0;

    virtual void registerPreferencesListener(
        std::shared_ptr< IPreferencesListener > & listener ) = 0;
    virtual void deregisterPreferencesListener(
        std::shared_ptr< IPreferencesListener > & listener ) = 0;

    /**
    * Loads default preferences from a file.
    * Will attempt to load preferences based on the plugin configuration file or global settings.
    * Will not load if files do not exist.
    * Will not load if any preferences already loaded (to prevent overriding any custom preferences set during initialise)
    * Will automatically save/create file if loaded.
    */
    virtual void loadDefaultPreferences() = 0;

    /**
    * Sets the folder preferences will save to
    * @param directory Passing empty will set the folder to default.
    * Directory will be created if doesn't exist. Note parent directory must exist.
    * @return if preferences folder was successfully set
    */
    virtual bool setPreferencesFolder( const char * directory ) = 0;

    /**
    * Sets the folder preferences will save to as the user directory
    * @return if preferences folder was successfully set
    */
    virtual bool setPreferencesFolderAsUserDirectory() = 0;

    /**
    * Save the current preferences to a file
    * @param fileName The name of the file to save with an optional extension.
    * Note if no extension is given, default extension is used
    */
    virtual void writePreferenceToFile( const char * fileName ) = 0;

    /**
    * Load preferences from a file
    * @param fileName The name of the file to load from with an optional extension.
    * Note if no extension is given, default extension is used
    */
    virtual void loadPreferenceFromFile( const char * fileName ) = 0;

    /**
    * Determines if the preference file exists
    * @param fileName The name of the file to check for with an optional extension.
    * Note if no extension is given, default extension is used
    */
    virtual bool hasPreferencesFile( const char * fileName ) const = 0;

};
} // end namespace wgt
#endif //I_PREFERENCE_HPP
