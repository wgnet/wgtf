#include "qt_preferences.hpp"

#include "core_common/assert.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_serialization/text_stream.hpp"
#include "core_serialization/i_file_system.hpp"
#include "core_serialization_xml/xml_serializer.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_qt_common/qt_global_settings.hpp"
#include "core_common/platform_path.hpp"
#include "core_string_utils/string_utils.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_generic_plugin/interfaces/i_command_line_parser.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_logging/logging.hpp"
#include <unordered_map>
#include <mutex>

namespace wgt
{
namespace
{
const char* s_globalPreferenceSuffix = "_global";
const char* s_localPreferenceSuffix = "_local";
const char* s_projectSettingsSuffix = "_project";
const char* s_projectSettingsSuffix_deprecated = "_viewport";
const char* s_preferenceExtension = ".settings";
const char* s_version = "1.0";
}

class QtPreferences::Implementation : Depends<IFileSystem, ICommandLineParser, IDefinitionManager, IQtFramework, IApplication>
{
public:
	Implementation(QtPreferences& qtPreferences)
	    : qtPreferences_(qtPreferences)
	{
		std::string customPrefFolder;
		std::string customPrefFile;
		if (auto commandLine = get<ICommandLineParser>())
		{
			customPrefFolder = commandLine->getParamStr("--preferenceFolder");
			customPrefFile   = commandLine->getParamStr("--config");

			if(!customPrefFile.empty())
			{
				customPrefFile = customPrefFile.substr(0, customPrefFile.rfind('.'));
			}
			else
			{
				std::string name = get<IApplication>()->getAppSettingsName();
				globalSettingsName_ = name + s_globalPreferenceSuffix + s_preferenceExtension;
				localSettingsName_ = name + s_localPreferenceSuffix + s_preferenceExtension;
			}
		}

		if (customPrefFolder.empty())
		{
			loadConfigFromNormalFolders(customPrefFile);
		}
		else
		{
			loadConfigFromCustomFolder(customPrefFolder, customPrefFile);
		}
		loadProjectPreferences();
	}

	~Implementation()
	{
		// NOTE: Preferences name is not set when a custom config file is set, but it does not exist. 
		// We don't create the config file on behalf of the user.
		if (!preferencesName_.empty())
		{
			saveCurrentPreferenceToFile(preferencesName_, false);	
		}
		saveProjectPreferences();
	}

	bool preferenceExists(const std::string& key, const std::string& preferenceKey = "") const;
	GenericObjectPtr getPreference(const std::string& key, const std::string& preferenceKey ="");

	void registerPreferencesListener(std::shared_ptr<IPreferencesListener>& listener);
	void deregisterPreferencesListener(std::shared_ptr<IPreferencesListener>& listener);

	bool saveCurrentPreferenceToFile(const std::string& fileName, bool notifyListeners = true);
	bool loadCurrentPreferenceFromFile(const std::string& fileName, bool notifyListeners = true);
	void loadDefaultPreferences();
	bool hasPreferenceFile(const std::string& fileName) const;
	bool setPreferencesFolder(const std::string& folderPath);

	void saveProjectPreferences();
	void loadProjectPreferences();

	void clearPreferences();

	void setDefaultPreferenceKey(const std::string& preferenceKey);
	const std::string& getDefaultPreferenceKey() const;

private:
	void loadConfigFromNormalFolders(const std::string &customPrefFile)
	{
		// Otherwise no custom pref folder given. Look in AppData for settings
		std::string appDataPathStr;
		bool appDataPathValid = false;
		if (GetWGAppDataPath(appDataPathStr))
		{
			appDataPathValid = setPreferencesFolder(appDataPathStr);
			if (appDataPathValid)
			{
				appDataPath_ = appDataPathStr;
				if (!customPrefFile.empty())
				{
					loadCurrentPreferenceFromFile(customPrefFile);
				}
				else
				{
					preferencesName_ = localSettingsName_;
					loadCurrentPreferenceFromFile(localSettingsName_);
					if (!hasLoadedPreferences_) 
					{
						loadCurrentPreferenceFromFile(globalSettingsName_);
					}
				}
			}

			if (hasLoadedPreferences_) return;
		}

		// Failed to load from AppData folder, old versions store in the executable folder
		// so check there and try migrate it to AppData folder if possible.
		std::string fallbackPath = getFallbackPrefPath();
		setPreferencesFolder(fallbackPath);

		auto fileSystem     = get <IFileSystem>();
		auto tryMigrateFile = [&](const std::string& fileName) -> void
		{
			if (hasLoadedPreferences_ && fileSystem && appDataPathValid)
			{
				std::string src  = fallbackPath + fileName + s_preferenceExtension;
				std::string dest = appDataPathStr + fileName + s_preferenceExtension;
				fileSystem->copy(src.c_str(), dest.c_str());
			}
		};

		if (!customPrefFile.empty())
		{
			loadCurrentPreferenceFromFile(customPrefFile);
			if (hasLoadedPreferences_ && fileSystem && appDataPathValid)
				tryMigrateFile(customPrefFile);
			else
				preferencesName_.clear();
		}
		else
		{
			loadCurrentPreferenceFromFile(localSettingsName_);
			if (hasLoadedPreferences_ && fileSystem && appDataPathValid)
			{
				tryMigrateFile(localSettingsName_);
			}

			// All fallbacks failed, load the default global settings editor is shipped with
			if (!hasLoadedPreferences_) loadDefaultPreferences();
			setPreferencesFolder(appDataPathStr);
		}
	}

	void loadConfigFromCustomFolder(const std::string& customPrefFolder, const std::string& customPrefFile) 
	{
		// NOTE: Custom locations/settings take precedence over everything. If they don't exist, we don't load
		// any config at all. The rationale is that when specifying a config, that there are no side effects and
		// the editor does exactly what it's been told to do.
		bool customPrefFolderValid = setPreferencesFolder(customPrefFolder);
		if (customPrefFolderValid) 
		{
			if (customPrefFile.empty()) 
			{
				preferencesName_ = localSettingsName_;
				loadCurrentPreferenceFromFile(localSettingsName_);
				if (!hasLoadedPreferences_) 
				{
					loadCurrentPreferenceFromFile(globalSettingsName_);
				}
			}
			else
			{
				loadCurrentPreferenceFromFile(customPrefFile);
			}
		}
		return;
	}

	std::string getProcessName()
	{
		char processPath[MAX_PATH];
		if(!GetModuleFileNameA(NULL, processPath, MAX_PATH))
		{
			assert(false && "Failed to get module file name!");
		}

		std::string processName = PathFindFileNameA(processPath);
		// We only want the executable name before .exe and the platform/configuration suffix.
		size_t len = processName.rfind('_');
		if(len == std::string::npos)
			len = processName.rfind(".exe");

		if(len != std::string::npos)
		{
			processName = processName.substr(0, len);
		}

		// Ensure lowercase for consistency
		for(size_t i = 0; i < processName.size(); ++i)
		{
			if(processName[i] >= 65 && processName[i] <= 90)
			{
				processName[i] += 32;
			}
		}

		return std::move(processName);
	}

	inline std::string getFallbackPrefPath()
	{
		wchar_t exePath[MAX_PATH];
		::GetModuleFileNameW(NULL, exePath, MAX_PATH);
		::PathRemoveFileSpecW(exePath);
		std::wstring resultW(exePath);

		std::string result(resultW.begin(), resultW.end());
		result += FilePath::kNativeDirectorySeparator;
		return result;
	}

	std::string getFullPreferenceFilePath(const std::string& fileName) const;

	typedef std::unordered_map<std::string, ManagedObject<GenericObject>> PreferenceData;

	QtPreferences& qtPreferences_;
	std::unordered_map<std::string, PreferenceData> preferences_;
	IPreferences::PreferencesListeners listeners_;
	mutable std::mutex mutex_;
	std::string preferencesFolder_;
	std::string preferencesName_;
	std::string localSettingsName_;
	std::string globalSettingsName_;
	std::string appDataPath_;
	std::string defaultPreferenceKey_ = "QtPreferences";
	bool hasLoadedPreferences_ = false;
};

bool QtPreferences::Implementation::preferenceExists(const std::string& key, const std::string& preferenceKey) const
{
	std::string prefKey;
	if (preferenceKey.empty())
	{
		prefKey = defaultPreferenceKey_;
	}
	else
	{
		prefKey = FilePath::getFileNoExtension(preferenceKey) + s_projectSettingsSuffix;
	}

	auto it = preferences_.find(prefKey);
	if (it != preferences_.end())
	{
		std::unique_lock<std::mutex> lock(mutex_);

		return it->second.find(key) != it->second.end();
	}

	return false;
}

GenericObjectPtr QtPreferences::Implementation::getPreference(const std::string& key, const std::string& preferenceKey)
{
	std::string prefKey;
	if (preferenceKey.empty())
	{
		prefKey = defaultPreferenceKey_;
	}
	else
	{
		prefKey = FilePath::getFileNoExtension(preferenceKey) + s_projectSettingsSuffix;
	}

	auto it = preferences_.find(prefKey);
	if (it != preferences_.end())
	{
		std::unique_lock<std::mutex> lock(mutex_);

		auto findIt = it->second.find(key);
		if (findIt != it->second.end())
		{
			auto& genericObject = findIt->second.getHandleT();
			if (genericObject != nullptr)
			{
				return genericObject;
			}
		}
	}

	auto definitionManager = get<IDefinitionManager>();
	TF_ASSERT(definitionManager != nullptr);
	auto insertIterator = preferences_[prefKey].insert_or_assign(key, GenericObject::create());
	return insertIterator.first->second.getHandleT();
}

void QtPreferences::Implementation::setDefaultPreferenceKey(const std::string& preferenceKey)
{
	defaultPreferenceKey_ = preferenceKey;
}

const std::string& QtPreferences::Implementation::getDefaultPreferenceKey() const
{
	return defaultPreferenceKey_;
}

void QtPreferences::Implementation::clearPreferences()
{
	preferences_[defaultPreferenceKey_] = PreferenceData();
}

void QtPreferences::Implementation::registerPreferencesListener(std::shared_ptr<IPreferencesListener>& listener)
{
	listeners_.push_back(listener);
}
void QtPreferences::Implementation::deregisterPreferencesListener(std::shared_ptr<IPreferencesListener>& listener)
{
	auto it = std::find(listeners_.begin(), listeners_.end(), listener);
	if (it != listeners_.end())
	{
		listeners_.erase(it);
	}
}

bool QtPreferences::Implementation::saveCurrentPreferenceToFile(const std::string& fileName, bool notifyListeners)
{
	auto definitionManager = get<IDefinitionManager>();
	auto fileSystem = get<IFileSystem>();
	TF_ASSERT(definitionManager && fileSystem);
	if (!definitionManager || !fileSystem)
	{
		return false;
	}

	const std::string filePath = PathIsRelative(StringUtils::to_wstring(fileName).c_str()) ?
		getFullPreferenceFilePath(fileName) : fileName;

	const std::string name = FilePath::getFileNoExtension(fileName);
	const bool isProjectPreference = name.find(s_projectSettingsSuffix) != std::string::npos;

	auto stream = fileSystem->readFile(filePath.c_str(), std::ios::out | std::ios::binary);
	if (!stream)
	{
		NGT_DEBUG_MSG("Could not save preferences to: %s", filePath.c_str());
		return false;
	}

	XMLSerializer serializer(*stream, *definitionManager);

	if (notifyListeners)
	{
		auto listeners = listeners_;
		auto itBegin = listeners.cbegin();
		auto itEnd = listeners.cend();
		for (auto it = itBegin; it != itEnd; ++it)
		{
			auto listener = *it;
			TF_ASSERT(listener != nullptr);
			listener->prePreferenceSaved();
		}
		auto qtFramework = get<IQtFramework>();
		TF_ASSERT(qtFramework != nullptr);
		auto qGlobalSettings = qtFramework->qtGlobalSettings();
		TF_ASSERT(qGlobalSettings != nullptr);
		qGlobalSettings->firePrePreferenceSavedEvent();
	}

	std::unique_lock<std::mutex> lock(mutex_);
	std::string version = "version ";
	version += s_version;
	serializer.serialize(version);	

	// Project preferences always use their name as a key
	const std::string preferenceKey = isProjectPreference ? name : defaultPreferenceKey_;

	auto it = preferences_.find(preferenceKey);
	if (it != preferences_.end())
	{
		definitionManager->serializeDefinitions(serializer);
		size_t size = it->second.size();
		serializer.serialize(size);
		for (auto& preferenceIter : it->second)
		{
			serializer.serialize(preferenceIter.first);
			serializer.serialize(preferenceIter.second.getHandleT());
		}
	}

	NGT_DEBUG_MSG("Saving preferences to: %s", filePath.c_str());
	return true;
}

void QtPreferences::Implementation::loadDefaultPreferences()
{
	this->setPreferencesFolder(getFallbackPrefPath());
	loadCurrentPreferenceFromFile(globalSettingsName_);
	preferencesName_ = localSettingsName_;
}

bool QtPreferences::Implementation::loadCurrentPreferenceFromFile(const std::string& fileName, bool notifyListeners)
{
	auto definitionManager = get<IDefinitionManager>();
	auto fileSystem = get<IFileSystem>();
	TF_ASSERT(definitionManager && fileSystem);
	if(!definitionManager || !fileSystem)
	{
		return false;
	}

	const std::string filePath = PathIsRelative(StringUtils::to_wstring(fileName).c_str()) ?
		getFullPreferenceFilePath(fileName) : fileName;
	std::string name = FilePath::getFileNoExtension(fileName);

	// Load any deprecated files but ensure saving using new suffix
	if (filePath.find(s_projectSettingsSuffix_deprecated) != std::string::npos)
	{
		std::string projFilePath = filePath;
		StringUtils::replace_string(projFilePath, s_projectSettingsSuffix_deprecated, s_projectSettingsSuffix);
		if (fileSystem->exists(projFilePath.c_str()))
		{
			return true; // Ignore as there is a proper project version
		}
		name = FilePath::getFileNoExtension(projFilePath);
	}

	const bool isProjectPreference = name.find(s_projectSettingsSuffix) != std::string::npos;

	if (!fileSystem->exists(filePath.c_str()))
	{
		NGT_DEBUG_MSG("Did not load preferences as %s does not exist", filePath.c_str());
		return false;
	}

	IFileSystem::IStreamPtr fileStream = fileSystem->readFile(filePath.c_str(), std::ios::in | std::ios::binary);
	XMLSerializer serializer(*fileStream, *definitionManager);

	auto listeners = listeners_;
	auto itBegin = listeners.cbegin();
	auto itEnd = listeners.cend();

	auto qtFramework = get<IQtFramework>();
	TF_ASSERT(qtFramework != nullptr);
	auto qGlobalSettings = qtFramework->qtGlobalSettings();
	TF_ASSERT(qGlobalSettings != nullptr);

	if (notifyListeners)
	{
		for (auto it = itBegin; it != itEnd; ++it)
		{
			auto listener = *it;
			TF_ASSERT(listener != nullptr);
			listener->prePreferencesChanged();
		}
		qGlobalSettings->firePrePreferenceChangeEvent();
	}

	std::unique_lock<std::mutex> lock(mutex_);
	std::string version;
	bool incorrectVersion = !serializer.deserialize(version) || version.length() < 11 || version.substr(0, 8) != "version " || version.substr(8) != s_version;

	if (incorrectVersion)
	{
		NGT_WARNING_MSG("Could not load preferences: incompatible version.");
		return false;
	}

	// Project preferences always use their name as a key
	const std::string preferenceKey = isProjectPreference ? name : defaultPreferenceKey_;

	preferences_[preferenceKey].clear();
	definitionManager->deserializeDefinitions(serializer);	
	version += s_version;
	serializer.serialize(version);

	size_t count = 0;
	serializer.deserialize(count);
	for (size_t i = 0; i < count; i++)
	{
		std::string key;
		bool isOk = serializer.deserialize(key);
		TF_ASSERT(isOk);
		ManagedObject<GenericObject> object;

		isOk = serializer.deserialize(object);
		if (isOk)
		{
			preferences_[preferenceKey].emplace(key, std::move(object));
		}
		else
		{
			continue;
		}
	}

	if (notifyListeners)
	{
		qGlobalSettings->firePostPreferenceChangeEvent();
		for (auto it = itBegin; it != itEnd; ++it)
		{
			auto listener = *it;
			TF_ASSERT(listener != nullptr);
			listener->postPreferencesChanged();
		}
	}

	if (!isProjectPreference)
	{
		hasLoadedPreferences_ = true;
	}

	NGT_DEBUG_MSG("Loading preferences from: %s", filePath.c_str());
	return true;
}

bool QtPreferences::Implementation::hasPreferenceFile(const std::string& fileName) const
{
	auto fileSystem = get<IFileSystem>();
	TF_ASSERT(fileSystem != nullptr);
	return fileSystem->exists(this->getFullPreferenceFilePath(fileName).c_str());
}

bool QtPreferences::Implementation::setPreferencesFolder(const std::string& folderPath)
{
	const std::string previousFolder = preferencesFolder_;
	preferencesFolder_ = folderPath;

	if (!preferencesFolder_.empty())
	{
		if (preferencesFolder_.back() != FilePath::kNativeDirectorySeparator &&
		    preferencesFolder_.back() != FilePath::kNativeAltDirectorySeparator)
		{
			preferencesFolder_ += FilePath::kNativeDirectorySeparator;
		}

		auto fileSystem = get<IFileSystem>();
		TF_ASSERT(fileSystem != nullptr);

		if (!fileSystem->exists(preferencesFolder_.c_str()) && !CreateDirectoryPath(preferencesFolder_.c_str()))
		{
			// Revert back to previous folder
			NGT_DEBUG_MSG("Preferences could not create folder: %s", preferencesFolder_.c_str());
			preferencesFolder_ = previousFolder;
			return false;
		}
	}
	return true;
}

std::string QtPreferences::Implementation::getFullPreferenceFilePath(const std::string& fileName) const
{
	// if no extension use the default one
	if (fileName.find('.') == std::string::npos)
	{
		return preferencesFolder_ + fileName + s_preferenceExtension;
	}
	return preferencesFolder_ + fileName;
}

void QtPreferences::Implementation::saveProjectPreferences()
{
	for (const auto& it : preferences_)
	{
		if (it.first.find(s_projectSettingsSuffix) != std::string::npos)
		{
			std::string filename = it.first + s_preferenceExtension;
			saveCurrentPreferenceToFile(it.first, false /*notifyListeners*/);
		}
	}
}

void QtPreferences::Implementation::loadProjectPreferences()
{
	auto fileSystem = get<IFileSystem>();
	if (fileSystem == nullptr)
	{
		return;
	}

	std::vector<std::string> paths;
	fileSystem->enumerate(appDataPath_.c_str(), [&](IFileInfoPtr&& fileInfo)
	{
		// Skip dots and folder
		if (fileInfo->isDots() || fileInfo->isDirectory())
		{
			return true;
		}

		std::string name = fileInfo->name().str();
		paths.emplace_back(name);
		return true;
	});

	for (const auto& i : paths)
	{
		if (i.find(s_projectSettingsSuffix_deprecated) != std::string::npos)
		{
			loadCurrentPreferenceFromFile(i.c_str(), false /*notifyListeners*/);
		}
		if (i.find(s_projectSettingsSuffix) != std::string::npos)
		{
			loadCurrentPreferenceFromFile(i.c_str(), false /*notifyListeners*/);
		}
	}
}

//------------------------------------------------------------------------------
QtPreferences::QtPreferences() : pImpl_(new Implementation(*this))
{
}

//------------------------------------------------------------------------------
QtPreferences::~QtPreferences()
{
	pImpl_ = nullptr;
}

//------------------------------------------------------------------------------
wgt::GenericObjectPtr QtPreferences::getPreference(const std::string& key, const std::string& preferenceKey)
{
	TF_ASSERT(pImpl_ != nullptr);
	return pImpl_->getPreference(key, preferenceKey);
}

bool QtPreferences::preferenceExists(const std::string& key, const std::string& preferenceKey) const
{
	TF_ASSERT(pImpl_ != nullptr);
	return pImpl_->preferenceExists(key, preferenceKey);
}

void QtPreferences::registerPreferencesListener(std::shared_ptr<IPreferencesListener>& listener)
{
	TF_ASSERT(pImpl_ != nullptr);
	return pImpl_->registerPreferencesListener(listener);
}

void QtPreferences::deregisterPreferencesListener(std::shared_ptr<IPreferencesListener>& listener)
{
	TF_ASSERT(pImpl_ != nullptr);
	return pImpl_->deregisterPreferencesListener(listener);
}

bool QtPreferences::writePreferenceToFile(const char* fileName, bool notifyListeners)
{
	TF_ASSERT(pImpl_ != nullptr);
	return pImpl_->saveCurrentPreferenceToFile(fileName, notifyListeners);
}

bool QtPreferences::loadPreferenceFromFile(const char* fileName, bool notifyListeners)
{
	TF_ASSERT(pImpl_ != nullptr);
	return pImpl_->loadCurrentPreferenceFromFile(fileName, notifyListeners);
}

void QtPreferences::loadDefaultPreferences()
{
	TF_ASSERT(pImpl_ != nullptr);
	return pImpl_->loadDefaultPreferences();
}

bool QtPreferences::hasPreferencesFile(const char* fileName) const
{
	TF_ASSERT(pImpl_ != nullptr);
	return pImpl_->hasPreferenceFile(fileName);
}

bool QtPreferences::setPreferencesFolder(const char* directory)
{
	TF_ASSERT(pImpl_ != nullptr);
	return pImpl_->setPreferencesFolder(directory);
}

bool QtPreferences::setPreferencesFolderAsUserDirectory()
{
	TF_ASSERT(pImpl_ != nullptr);

	std::string preferencesFolder;
	char userDirectoryPath[MAX_PATH];

	if (GetUserDirectoryPath(userDirectoryPath))
	{
		preferencesFolder = userDirectoryPath;
		NGT_DEBUG_MSG("Preferences found user directory: %s", preferencesFolder.c_str());
		preferencesFolder += FilePath::kNativeDirectorySeparator;
		preferencesFolder += "wgtools";
		return pImpl_->setPreferencesFolder(preferencesFolder);
	}

	NGT_DEBUG_MSG("Preferences could not find user directory");
	return false;
}

const char* QtPreferences::getPreferencesExtension() const
{
	return s_preferenceExtension;
}

void QtPreferences::setDefaultPreferenceKey(const std::string& preferenceKey)
{
	pImpl_->setDefaultPreferenceKey(preferenceKey);
}

const std::string& QtPreferences::getDefaultPreferenceKey() const
{
	return pImpl_->getDefaultPreferenceKey();
}

void QtPreferences::clearPreferences()
{
	pImpl_->clearPreferences();
}

} // end namespace wgt
