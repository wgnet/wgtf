#include "qt_preferences.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_serialization/text_stream.hpp"
#include "core_serialization/i_file_system.hpp"
#include "core_serialization/serializer/xml_serializer.hpp"
#include "core_qt_common/i_qt_framework.hpp"  
#include "core_qt_common/qt_global_settings.hpp"
#include "core_command_system/i_env_system.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_generic_plugin/interfaces/i_command_line_parser.hpp"
#include <unordered_map>
#include <mutex>

namespace wgt
{
namespace 
{
    const char* s_globalPreference = "global_setting";
    const char* s_preferenceExtension = ".settings";
}

class QtPreferences::Implementation
{
public:
    Implementation( IComponentContext& contextManager, QtPreferences& qtPreferences)
        : contextManager_( contextManager )
        , qtPreferences_(qtPreferences)
        , defaultPreferencesName_( s_globalPreference )
        , hasLoadedPreferences_( false )
    {
        auto fileSystem = contextManager_.queryInterface< IFileSystem >();
        assert( fileSystem != nullptr );

        ICommandLineParser* commandLine = contextManager.queryInterface<ICommandLineParser>();
        if (commandLine != nullptr)
        {
            this->setpreferencesFolder(commandLine->getParamStr("preferenceFolder"), *fileSystem);

            const auto config = commandLine->getParamStr( "--config" );
            if( !config.empty() )
            {
                defaultPreferencesName_ = config.substr( 0, config.rfind( '.' ) );
            }
        }
    }

    ~Implementation()
    {
        this->saveCurrentPreferenceToFile( defaultPreferencesName_, false );
    }

    bool preferenceExists( const char* key ) const;
    GenericObjectPtr & getPreference( const char * key );

    void registerPreferencesListener( std::shared_ptr< IPreferencesListener > & listener );
    void deregisterPreferencesListener( std::shared_ptr< IPreferencesListener > & listener );

    void saveCurrentPreferenceToFile( const std::string& fileName, bool notifyListeners );
    void loadCurrentPreferenceFromFile( const std::string& fileName );
    void loadDefaultPreferences();
    bool hasPreferenceFile( const std::string& fileName ) const;

private:

    void setpreferencesFolder( std::string&& folderPath, IFileSystem& fileSystem );
    void setPreferencesGlobalName( const std::string& fileName );
    std::string getFullPreferenceFilePath( const std::string& fileName ) const;

    IComponentContext& contextManager_;
    QtPreferences& qtPreferences_;
    std::unordered_map< std::string, GenericObjectPtr > preferences_;
    IPreferences::PreferencesListeners listeners_;
    mutable std::mutex mutex_;
    std::string preferencesFolder_;
    std::string defaultPreferencesName_;
    bool hasLoadedPreferences_;
};

bool QtPreferences::Implementation::preferenceExists( const char* key ) const
{
    std::unique_lock<std::mutex> lock( mutex_ );

    return preferences_.find( key ) != preferences_.end();
}

GenericObjectPtr & QtPreferences::Implementation::getPreference( const char * key )
{
    std::unique_lock<std::mutex> lock( mutex_ );

    auto findIt = preferences_.find( key );
    if (findIt != preferences_.end())
    {
        return findIt->second;
    }

    auto definitionManager = contextManager_.queryInterface< IDefinitionManager >();
    assert( definitionManager != nullptr );
    auto preference = GenericObject::create( *definitionManager );
    preferences_[ key ] = preference;
    return preferences_[ key ];
}

void QtPreferences::Implementation::registerPreferencesListener( std::shared_ptr< IPreferencesListener > & listener )
{
    listeners_.push_back( listener );
}
void QtPreferences::Implementation::deregisterPreferencesListener( std::shared_ptr< IPreferencesListener > & listener )
{
    listeners_.erase( listener );
}

void QtPreferences::Implementation::saveCurrentPreferenceToFile( const std::string& fileName, bool notifyListeners )
{
    const std::string filePath = this->getFullPreferenceFilePath( fileName );

    auto definitionManager = contextManager_.queryInterface< IDefinitionManager >();
    auto fileSystem = contextManager_.queryInterface< IFileSystem >();
    assert( definitionManager && fileSystem );

    auto stream = fileSystem->readFile( filePath.c_str(), std::ios::out | std::ios::binary );
    if( !stream )
    {
        NGT_DEBUG_MSG( ( "Could not save preferences to: " + filePath + "\n" ).c_str() );
        return;
    }

    XMLSerializer serializer( *stream, *definitionManager );

    if( notifyListeners )
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
    }

    {
        std::unique_lock<std::mutex> lock( mutex_ );
        definitionManager->serializeDefinitions( serializer );
        size_t size = preferences_.size();
        serializer.serialize( size );
        for( auto& preferenceIter : preferences_ )
        {
            serializer.serialize( preferenceIter.first );
            serializer.serialize( preferenceIter.second );
        }
    }

    NGT_DEBUG_MSG( ( "Saving preferences to: " + filePath + "\n" ).c_str() );
}

void QtPreferences::Implementation::loadDefaultPreferences()
{
    if( !hasLoadedPreferences_ )
    {
        if( this->hasPreferenceFile( defaultPreferencesName_ ) )
        {
            this->loadCurrentPreferenceFromFile( defaultPreferencesName_ );
        }
        else if( this->hasPreferenceFile( s_globalPreference ) )
        {
            this->loadCurrentPreferenceFromFile( s_globalPreference );
        }
    }
}

void QtPreferences::Implementation::loadCurrentPreferenceFromFile( const std::string& fileName )
{
    const std::string filePath = this->getFullPreferenceFilePath( fileName );

    auto definitionManager = contextManager_.queryInterface< IDefinitionManager >();
    auto fileSystem = contextManager_.queryInterface< IFileSystem >();
    assert( definitionManager && fileSystem );

    if( !fileSystem->exists( filePath.c_str() ) )
    {
        NGT_DEBUG_MSG( ( "Did not load preferences as " + filePath + " does not exist\n" ).c_str() );
        return;
    }

    IFileSystem::IStreamPtr fileStream = 
        fileSystem->readFile(filePath.c_str(), std::ios::in | std::ios::binary );
    XMLSerializer serializer( *fileStream, *definitionManager );

    auto itBegin = listeners_.cbegin();
    auto itEnd = listeners_.cend();
    for( auto it = itBegin; it != itEnd; ++it )
    {
        auto listener = it->lock();
        assert( listener != nullptr );
        listener->prePreferencesChanged();
    }

    auto qtFramework = contextManager_.queryInterface< IQtFramework >();
    assert( qtFramework != nullptr );
    auto qGlobalSettings = qtFramework->qtGlobalSettings();
    assert( qGlobalSettings != nullptr );
    qGlobalSettings->firePrePreferenceChangeEvent();

    {
        std::unique_lock<std::mutex> lock( mutex_ );
        auto definitionManager = contextManager_.queryInterface< IDefinitionManager >();
        assert( definitionManager );

        preferences_.clear();
        definitionManager->deserializeDefinitions( serializer );

        size_t count = 0;
        serializer.deserialize( count );
        for (size_t i = 0; i < count; i++)
        {
            std::string key;
            serializer.deserialize( key );

            const MetaType * metaType = 
                Variant::findType( getClassIdentifier<ObjectHandle>() );
            assert( metaType != nullptr );
            Variant value( metaType );
            serializer.deserialize( value );
            GenericObjectPtr obj;
            bool isOk = value.tryCast( obj );
            assert( isOk );
            preferences_[ key ] = obj;
        }
    }

    qGlobalSettings->firePostPreferenceChangeEvent();
    for( auto it = itBegin; it != itEnd; ++it )
    {
        auto listener = it->lock();
        assert( listener != nullptr );
        listener->postPreferencesChanged();
    }

    hasLoadedPreferences_ = true;

    NGT_DEBUG_MSG( ( "Loading preferences from: " + filePath + "\n" ).c_str() );
}

bool QtPreferences::Implementation::hasPreferenceFile( const std::string& fileName ) const
{
    auto fileSystem = contextManager_.queryInterface< IFileSystem >();
    assert( fileSystem != nullptr );
    return fileSystem->exists( this->getFullPreferenceFilePath( fileName ).c_str() );
}

void QtPreferences::Implementation::setpreferencesFolder(std::string&& folderPath, IFileSystem& fileSystem)
{
    preferencesFolder_ = std::move(folderPath);
	if (!preferencesFolder_.empty())
	{
		if (preferencesFolder_.back() != '/')
		{
			preferencesFolder_ += '/';
		}

		if (!fileSystem.exists( preferencesFolder_.c_str() ))
		{
			preferencesFolder_ = std::string();
		}
	}
}

std::string QtPreferences::Implementation::getFullPreferenceFilePath( const std::string& fileName ) const
{
    // if no extension use the default one
    if( fileName.find( '.' ) == std::string::npos )
    {
        return preferencesFolder_ + fileName + s_preferenceExtension;
    }
    return preferencesFolder_ + fileName;
}

//------------------------------------------------------------------------------
QtPreferences::QtPreferences( IComponentContext& contextManager )
{
    pImpl_.reset( new Implementation( contextManager, *this ) );
}

//------------------------------------------------------------------------------
QtPreferences::~QtPreferences()
{
    pImpl_ = nullptr;
}

//------------------------------------------------------------------------------
GenericObjectPtr & QtPreferences::getPreference( const char * key )
{
    assert( pImpl_ != nullptr );
    return pImpl_->getPreference( key );
}

bool QtPreferences::preferenceExists( const char* key ) const
{
    assert( pImpl_ != nullptr );
    return pImpl_->preferenceExists( key );
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

void QtPreferences::writePreferenceToFile( const char * fileName )
{
    assert( pImpl_ != nullptr );
    return pImpl_->saveCurrentPreferenceToFile( fileName, true );
}

void QtPreferences::loadPreferenceFromFile( const char * fileName )
{
    assert( pImpl_ != nullptr );
    return pImpl_->loadCurrentPreferenceFromFile( fileName );
}

void QtPreferences::loadDefaultPreferences()
{
    assert( pImpl_ != nullptr );
    return pImpl_->loadDefaultPreferences();
}

bool QtPreferences::hasPreferencesFile( const char * fileName ) const
{
    assert( pImpl_ != nullptr );
    return pImpl_->hasPreferenceFile( fileName );
}
} // end namespace wgt
