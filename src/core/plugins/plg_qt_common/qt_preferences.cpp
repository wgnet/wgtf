#include "qt_preferences.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_serialization/text_stream.hpp"
#include "core_serialization/i_file_system.hpp"
#include "core_serialization/serializer/xml_serializer.hpp"
#include "core_variant/interfaces/i_meta_type_manager.hpp"
#include "core_qt_common/i_qt_framework.hpp"  
#include "core_qt_common/qt_global_settings.hpp"
#include "core_command_system/i_env_system.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include <unordered_map>
#include <mutex>

namespace wgt
{
namespace 
{
    const char* s_globalPreference = "global_setting.settings";
	const char* s_preferenceExtension = ".settings";
    std::string genProjectSettingName( const char * projectName )
    {
        std::string s = projectName;
        s += s_preferenceExtension;
        return s;
    }

	class PreferenceEnvCom : public IEnvComponent
	{
		DEFINE_EC_GUID
	public:
		PreferenceEnvCom()
		{
		}

		virtual ~PreferenceEnvCom()
		{
		}

		std::unordered_map< std::string, GenericObjectPtr > preferences_;
	};

	DECLARE_EC_GUID( PreferenceEnvCom, 0xe57cac3c, 0x44dc0793, 0x4a385655, 0xc18f231c );



}

class QtPreferences::Implementation : public IEnvEventListener
{
public:

    Implementation( IComponentContext& contextManager, QtPreferences& qtPreferences)
        : contextManager_( contextManager )
        , qtPreferences_(qtPreferences)
        , preferenceState_( &globalPreferenceState_ )
    {
        auto fileSystem = contextManager_.queryInterface< IFileSystem >();
        assert( fileSystem != nullptr );
        if (fileSystem->exists( s_globalPreference ))
        {
            auto definitionManager = contextManager_.queryInterface< IDefinitionManager >();
            assert( definitionManager != nullptr );
            IFileSystem::IStreamPtr fileStream = 
                fileSystem->readFile( s_globalPreference, std::ios::in | std::ios::binary );
            XMLSerializer serializer( *fileStream, *definitionManager );
            loadPreferenceState( serializer, preferenceState_ );
        }
        auto em = contextManager_.queryInterface<IEnvManager>();
        assert( em != nullptr );
        em->registerListener( this );
    }

    ~Implementation()
    {
        auto fileSystem = contextManager_.queryInterface< IFileSystem >();
        assert( fileSystem != nullptr );
        auto stream = fileSystem->readFile( s_globalPreference, std::ios::out | std::ios::binary );
        if(stream)
        {
            auto definitionManager = contextManager_.queryInterface< IDefinitionManager >();
            assert( definitionManager != nullptr );
            XMLSerializer serializer( *stream, *definitionManager );
            savePreferenceState( serializer, preferenceState_ );
        }
        auto em = contextManager_.queryInterface<IEnvManager>();
        assert( em != nullptr );
        em->deregisterListener( this );
    }

    GenericObjectPtr & getPreference( const char * key );

    void registerPreferencesListener( std::shared_ptr< IPreferencesListener > & listener );
    void deregisterPreferencesListener( std::shared_ptr< IPreferencesListener > & listener );

    void saveCurrentPreferenceToFile( const char * filePath );
    void loadCurrentPreferenceFromFile( const char * filePath );
    void savePreferenceState(  ISerializer & serializer, const PreferenceEnvCom* ec );
    void loadPreferenceState(  ISerializer & serializer, PreferenceEnvCom* ec );

    // IEnvEventListener
    virtual void onAddEnv( IEnvState* state ) override;

    virtual void onRemoveEnv( IEnvState* state ) override;

    virtual void onSelectEnv( IEnvState* state ) override;

    virtual void onSaveEnvState( IEnvState* state ) override;

    virtual void onLoadEnvState( IEnvState* state ) override;

private:
    void switchEnvContext(PreferenceEnvCom* ec);

    IComponentContext& contextManager_;
    QtPreferences& qtPreferences_;
    PreferenceEnvCom globalPreferenceState_;
    PreferenceEnvCom* preferenceState_;
    IPreferences::PreferencesListeners listeners_;
    std::mutex								mutex_;

};

GenericObjectPtr & QtPreferences::Implementation::getPreference( const char * key )
{
    std::unique_lock<std::mutex> lock( mutex_ );
    auto definitionManager = contextManager_.queryInterface< IDefinitionManager >();
    assert( definitionManager != nullptr );
    auto findIt = preferenceState_->preferences_.find( key );
    if (findIt != preferenceState_->preferences_.end())
    {
        return findIt->second;
    }
    auto preference = GenericObject::create( *definitionManager );
    preferenceState_->preferences_[ key ] = preference;
    return preferenceState_->preferences_[ key ];
}

void QtPreferences::Implementation::registerPreferencesListener( std::shared_ptr< IPreferencesListener > & listener )
{
    listeners_.push_back( listener );
}
void QtPreferences::Implementation::deregisterPreferencesListener( std::shared_ptr< IPreferencesListener > & listener )
{
    listeners_.erase( listener );
}

void QtPreferences::Implementation::saveCurrentPreferenceToFile( const char * filePath )
{
    auto definitionManager = contextManager_.queryInterface< IDefinitionManager >();
    auto fileSystem = contextManager_.queryInterface< IFileSystem >();
    assert( definitionManager && fileSystem );
    auto stream = fileSystem->readFile( filePath, std::ios::out | std::ios::binary );
    if(stream)
    {
        XMLSerializer serializer( *stream, *definitionManager );
        savePreferenceState( serializer, preferenceState_ );
    }
}

void QtPreferences::Implementation::loadCurrentPreferenceFromFile( const char * filePath )
{
    auto definitionManager = contextManager_.queryInterface< IDefinitionManager >();
    auto fileSystem = contextManager_.queryInterface< IFileSystem >();
    assert( definitionManager && fileSystem );
    if(!fileSystem->exists( filePath ))
    {
        assert( false );
        return;
    }
    IFileSystem::IStreamPtr fileStream = 
        fileSystem->readFile( filePath, std::ios::in | std::ios::binary );
    XMLSerializer serializer( *fileStream, *definitionManager );
    loadPreferenceState( serializer, preferenceState_ );

}

void QtPreferences::Implementation::savePreferenceState(  ISerializer & serializer, 
                                                          const PreferenceEnvCom* ec )
{
    auto itBegin = listeners_.cbegin();
    auto itEnd = listeners_.cend();
    for( auto it = itBegin; it != itEnd; ++it )
    {
        auto listener = it->lock();
        assert( listener != nullptr );
        listener->prePreferenceSaved();
    }
    auto qtFramework = contextManager_.queryInterface< IQtFramework >();
    assert( qtFramework != nullptr );
    auto qGlobalSettings = qtFramework->qtGlobalSettings();
    assert( qGlobalSettings != nullptr );
    qGlobalSettings->firePrePreferenceSavedEvent();

    std::unique_lock<std::mutex> lock( mutex_ );
    auto definitionManager = contextManager_.queryInterface< IDefinitionManager >();
    assert( definitionManager != nullptr );
    definitionManager->serializeDefinitions( serializer );
    size_t size = ec->preferences_.size();
    serializer.serialize( size );
    for( auto& preferenceIter : ec->preferences_ )
    {
        serializer.serialize( preferenceIter.first );
        serializer.serialize( preferenceIter.second );
    }
}
void QtPreferences::Implementation::loadPreferenceState(  ISerializer & serializer, 
                                                          PreferenceEnvCom* ec )
{
    std::unique_lock<std::mutex> lock( mutex_ );
    auto definitionManager = contextManager_.queryInterface< IDefinitionManager >();
    auto metaTypeManager = contextManager_.queryInterface<IMetaTypeManager>();
    assert( definitionManager && metaTypeManager );
    ec->preferences_.clear();
    definitionManager->deserializeDefinitions( serializer );
    size_t count = 0;
    serializer.deserialize( count );
    for (size_t i = 0; i < count; i++)
    {
        std::string key;
        serializer.deserialize( key );

        const MetaType * metaType = 
            metaTypeManager->findType( getClassIdentifier<ObjectHandle>() );
        assert( metaType != nullptr );
        Variant value( metaType );
        serializer.deserialize( value );
        GenericObjectPtr obj;
        bool isOk = value.tryCast( obj );
        assert( isOk );
        ec->preferences_[ key ] = obj;
    }
}

void QtPreferences::Implementation::onAddEnv( IEnvState* state ) 
{
    ENV_STATE_ADD( PreferenceEnvCom, ec );
}

void QtPreferences::Implementation::onRemoveEnv( IEnvState* state )
{
    ENV_STATE_REMOVE( PreferenceEnvCom, ec );
    if (ec == preferenceState_)
    {
        switchEnvContext( &globalPreferenceState_ );
    }
}

void QtPreferences::Implementation::onSelectEnv( IEnvState* state ) 
{
    ENV_STATE_QUERY( PreferenceEnvCom, ec );
    if (ec != preferenceState_)
    {
        switchEnvContext(ec);
    }
}

void QtPreferences::Implementation::onSaveEnvState( IEnvState* state ) 
{
    ENV_STATE_QUERY( PreferenceEnvCom, ec );
    std::string settings = genProjectSettingName( state->description() );
    auto definitionManager = contextManager_.queryInterface< IDefinitionManager >();
    auto fileSystem = contextManager_.queryInterface< IFileSystem >();
    assert( definitionManager && fileSystem );
    auto stream = fileSystem->readFile( settings.c_str(), std::ios::out | std::ios::binary );
    if(stream)
    {
        XMLSerializer serializer( *stream, *definitionManager );
        savePreferenceState( serializer, ec );
    }
}

void QtPreferences::Implementation::onLoadEnvState( IEnvState* state ) 
{
    ENV_STATE_QUERY( PreferenceEnvCom, ec );
    std::string settings = genProjectSettingName( state->description() );
    auto definitionManager = contextManager_.queryInterface< IDefinitionManager >();
    auto fileSystem = contextManager_.queryInterface< IFileSystem >();
    assert( definitionManager && fileSystem );
    auto stream = fileSystem->readFile( settings.c_str(), std::ios::in | std::ios::binary );
    if(stream)
    {
        XMLSerializer serializer( *stream, *definitionManager );
        loadPreferenceState( serializer, ec );
    }
}

void QtPreferences::Implementation::switchEnvContext(PreferenceEnvCom* ec)
{
    auto qtFramework = contextManager_.queryInterface< IQtFramework >();
    assert( qtFramework != nullptr );
    auto qGlobalSettings = qtFramework->qtGlobalSettings();
    assert( qGlobalSettings != nullptr );

    auto itBegin = listeners_.cbegin();
    auto itEnd = listeners_.cend();
    for( auto it = itBegin; it != itEnd; ++it )
    {
        auto listener = it->lock();
        assert( listener != nullptr );
        listener->prePreferencesChanged();
    }
    qGlobalSettings->firePrePreferenceChangeEvent();
    if(preferenceState_ == &globalPreferenceState_)
    {
        saveCurrentPreferenceToFile( s_globalPreference );
    }
    {
        std::unique_lock<std::mutex> lock( mutex_ );
        preferenceState_ = ec;
    }
    if(preferenceState_ == &globalPreferenceState_)
    {
        loadCurrentPreferenceFromFile( s_globalPreference );
    }
    for( auto it = itBegin; it != itEnd; ++it )
    {
        auto listener = it->lock();
        assert( listener != nullptr );
        listener->postPreferencesChanged();
    }

    qGlobalSettings->firePostPreferenceChangeEvent();
}

//------------------------------------------------------------------------------
QtPreferences::QtPreferences()
{
}

//------------------------------------------------------------------------------
QtPreferences::~QtPreferences()
{
}

//------------------------------------------------------------------------------
void QtPreferences::init( IComponentContext& contextManager )
{
    pImpl_.reset( new Implementation( contextManager, *this ) );
}

//------------------------------------------------------------------------------
void QtPreferences::fini()
{
    pImpl_ = nullptr;
}

//------------------------------------------------------------------------------
GenericObjectPtr & QtPreferences::getPreference( const char * key )
{
    assert( pImpl_ != nullptr );
    return pImpl_->getPreference( key );
}

void QtPreferences::registerPreferencesListener( std::shared_ptr< IPreferencesListener > & listener )
{
    assert( pImpl_ != nullptr );
    return pImpl_->registerPreferencesListener( listener );
}

void QtPreferences::deregisterPreferencesListener( std::shared_ptr< IPreferencesListener > & listener )
{
    assert( pImpl_ != nullptr );
    return pImpl_->deregisterPreferencesListener( listener );
}

void QtPreferences::writePreferenceToFile( const char * filePath )
{
    assert( pImpl_ != nullptr );
    return pImpl_->saveCurrentPreferenceToFile( filePath );
}

void QtPreferences::loadPreferenceFromFile( const char * filePath )
{
    assert( pImpl_ != nullptr );
    return pImpl_->loadCurrentPreferenceFromFile( filePath );
}
} // end namespace wgt
