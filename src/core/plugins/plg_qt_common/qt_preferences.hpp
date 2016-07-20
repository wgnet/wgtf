#ifndef QT_PREFERENCES_HPP
#define QT_PREFERENCES_HPP

#include "core_ui_framework/i_preferences.hpp"
#include "core_dependency_system/i_interface.hpp"

namespace wgt
{
class IComponentContext;

class QtPreferences : public Implements< IPreferences >
{
public:
	QtPreferences( IComponentContext& contextManager );
	~QtPreferences();

    virtual GenericObjectPtr & getPreference( const char * key ) override;
    virtual bool preferenceExists( const char* key ) const override;
    virtual void registerPreferencesListener( std::shared_ptr< IPreferencesListener > & listener ) override;
    virtual void deregisterPreferencesListener( std::shared_ptr< IPreferencesListener > & listener ) override;
    virtual void writePreferenceToFile( const char * fileName ) override;
    virtual void loadPreferenceFromFile( const char * fileName ) override;
    virtual bool hasPreferencesFile( const char * fileName ) const override;
    virtual void loadDefaultPreferences() override;

private:
    class Implementation;
    std::unique_ptr< Implementation > pImpl_;
};
} // end namespace wgt
#endif//QT_PREFERENCES_HPP
