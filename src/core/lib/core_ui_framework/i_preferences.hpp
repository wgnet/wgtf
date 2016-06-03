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
    virtual void registerPreferencesListener(
        std::shared_ptr< IPreferencesListener > & listener ) = 0;
    virtual void deregisterPreferencesListener(
        std::shared_ptr< IPreferencesListener > & listener ) = 0;

    virtual void writePreferenceToFile( const char * filePath ) {}
    virtual void loadPreferenceFromFile( const char * filePath ) {}


};
} // end namespace wgt
#endif //I_PREFERENCE_HPP
