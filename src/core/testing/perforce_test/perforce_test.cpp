#include "stdafx.h"
#include "core_generic_plugin_manager/generic_plugin_manager.hpp"
#include "core_generic_plugin/interfaces/i_plugin_context_manager.hpp"

#include "interfaces/version_control/i_version_control.hpp"
#include "interfaces/version_control/i_depot_view.hpp"
#include "core_string_utils/string_utils.hpp"

#include <locale>
#include <codecvt>
#include <string>

#include "CppUnitTest.h"
#include "core_string_utils/file_path.hpp"

namespace wgt
{
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
EXTERN_C IMAGE_DOS_HEADER __ImageBase;

namespace UnitTests
{
	/*! As far as I can tell Visual Studio's CppUnitTestFramework doesn't like what we are doing with our allocator
		In order to improve the results we store our data in a separate object and use TEST_CLASS_INITIALIZE/CLEANUP
		This at least allows the framework to return whether or not the test passed in a Release build (Debug works)
		In Release the framework hangs for a while and eventually complains after the tests complete running.
		When in Debug there are first chance exceptions but it appears to shutdown
	*/
	struct PerforceUnitTestData
	{
		GenericPluginManager pluginManager;
		std::string thisFile;
		std::string tempFile;
		IVersionControl* versionControl;
		IDepotViewPtr depotView;
		std::wstring_convert< Utf16to8Facet > converter;

		PerforceUnitTestData() : converter( Utf16to8Facet::create() )
		{
			// Grab this file's location for later testing purposes
			thisFile = __FILE__;
			auto dir = thisFile.substr(0, thisFile.rfind('\\'));
			// Create a temporary file for later testing purposes
			char tempFilePath[MAX_PATH] = { 0 };
			::GetTempFileNameA(dir.c_str(), "P4T", 0, tempFilePath);
			tempFile = tempFilePath;

			// Load the perforce plugin, located relative to the location of this .dll
			std::vector<std::wstring> plugins;
			LPTSTR  modulePath = new TCHAR[_MAX_PATH];
			::GetModuleFileName((HINSTANCE)&__ImageBase, modulePath, _MAX_PATH);
			std::wstring basePath(modulePath);
			basePath = basePath.substr(0, basePath.rfind('\\'));
			plugins.emplace_back(basePath + L"/plugins/plg_perforce");
			pluginManager.loadPlugins(plugins);

			// Retrieve perforce information from the registry
			HKEY hKey;
			Assert::IsTrue(ERROR_SUCCESS == RegOpenKeyA(HKEY_CURRENT_USER, "Software\\Perforce\\environment", &hKey), L"Failed to open Perforce registry key");
			char client[MAX_PATH] = { 0 };
			char password[MAX_PATH] = { 0 };
			char port[MAX_PATH] = { 0 };
			char user[MAX_PATH] = { 0 };
			DWORD bufferSize = MAX_PATH;
			Assert::IsTrue(ERROR_SUCCESS == RegQueryValueExA(hKey, "P4CLIENT", 0, NULL, (BYTE*)client, &bufferSize), L"Failed to retrieve P4CLIENT from registry");
			bufferSize = MAX_PATH;
			Assert::IsTrue(ERROR_SUCCESS == RegQueryValueExA(hKey, "P4PASSWD", 0, NULL, (BYTE*)password, &bufferSize), L"Failed to retrieve P4PASSWD from registry");
			bufferSize = MAX_PATH;
			Assert::IsTrue(ERROR_SUCCESS == RegQueryValueExA(hKey, "P4PORT", 0, NULL, (BYTE*)port, &bufferSize), L"Failed to retrieve P4PORT from registry");
			bufferSize = MAX_PATH;
			Assert::IsTrue(ERROR_SUCCESS == RegQueryValueExA(hKey, "P4USER", 0, NULL, (BYTE*)user, &bufferSize), L"Failed to retrieve P4USER from registry");
			::RegCloseKey(hKey);

			// Initialize the version control interface
			versionControl = pluginManager.getContextManager().getGlobalContext()->queryInterface<IVersionControl>();
			Assert::IsNotNull(versionControl, L"Failed to get IVersionControl Interface");
			auto result = versionControl->initialize("", port, user, client, password);
			Assert::IsTrue(!result->hasErrors(), converter.from_bytes(result->errors()).c_str());

			// Create a view of the depot
			depotView = versionControl->createDepotView("", "");
			Assert::IsNotNull(depotView.get(), L"Failed to create depot view");
		}
		~PerforceUnitTestData()
		{
			::DeleteFileA(tempFile.c_str());
		}
	};

	std::unique_ptr<PerforceUnitTestData> d;

	TEST_CLASS(PerforceUnitTests)
	{
	public:
		TEST_CLASS_INITIALIZE(initialize)
		{
			d.reset(new PerforceUnitTestData());
		}

		TEST_CLASS_CLEANUP(cleanup)
		{
			d.reset();
		}

		TEST_METHOD(CreateChangeList)
		{
			// Create an empty changelist
			IDepotView::ChangeListId changelist;
			auto result = d->depotView->createChangeList("Multi\rLine\nTest\r\nChangelist", changelist);
			Assert::IsTrue(!result->hasErrors(), d->converter.from_bytes(result->errors()).c_str());

			// Delete the change list
			result = d->depotView->deleteEmptyChangeList(changelist);
			Assert::IsTrue(!result->hasErrors(), d->converter.from_bytes(result->errors()).c_str());
		}

		TEST_METHOD(AddAndRevertFile)
		{
			// Create an empty changelist
			IDepotView::ChangeListId changelist;
			auto result = d->depotView->createChangeList("Multi\rLine\nTest\r\nChangelist", changelist);
			Assert::IsTrue(!result->hasErrors(), d->converter.from_bytes(result->errors()).c_str());

			// Add the temp file to the newly created change list
			IDepotView::PathList paths;
			paths.emplace_back(d->tempFile);
			result = d->depotView->add(paths, changelist);
			Assert::IsTrue(!result->hasErrors(), d->converter.from_bytes(result->errors()).c_str());

			// Revert the file
			result = d->depotView->revert(paths);
			Assert::IsTrue(!result->hasErrors(),d->converter.from_bytes(result->errors()).c_str());

			// Delete the change list
			result = d->depotView->deleteEmptyChangeList(changelist);
			Assert::IsTrue(!result->hasErrors(),d->converter.from_bytes(result->errors()).c_str());
		}

		TEST_METHOD(CheckoutFile)
		{
			IDepotView::PathList paths;
			paths.emplace_back(d->thisFile);
			auto result = d->depotView->status(paths);
			Assert::IsTrue(!result->hasErrors(),d->converter.from_bytes(result->errors()).c_str());

			Assert::IsTrue(std::string(result->output()).find("action") == std::string::npos, L"Can't complete test, file already checked out");
			
			// Create an empty changelist
			IDepotView::ChangeListId changelist;
			result =d->depotView->createChangeList("Unit Test Checkout", changelist);
			Assert::IsTrue(!result->hasErrors(),d->converter.from_bytes(result->errors()).c_str());

			// Checkout the file
			result =d->depotView->checkout(paths, changelist);
			Assert::IsTrue(!result->hasErrors(),d->converter.from_bytes(result->errors()).c_str());
			Assert::IsTrue(std::string(result->output()).find("action edit") != std::string::npos);

			// Revert the file
			result =d->depotView->revert(paths);
			Assert::IsTrue(!result->hasErrors(),d->converter.from_bytes(result->errors()).c_str());

			// Delete the change list
			result =d->depotView->deleteEmptyChangeList(changelist);
			Assert::IsTrue(!result->hasErrors(),d->converter.from_bytes(result->errors()).c_str());
		}

		TEST_METHOD(GetFileRevisions)
		{
			IDepotView::PathList paths;
			paths.emplace_back(d->thisFile);
			auto result =d->depotView->status(paths);
			Assert::IsTrue(!result->hasErrors(),d->converter.from_bytes(result->errors()).c_str());

			Assert::IsTrue(std::string(result->output()).find("action") == std::string::npos, L"Can't complete test, file already checked out");

			// Get the first version of this file
			result =d->depotView->get(paths, 1);
			Assert::IsTrue(!result->hasErrors(),d->converter.from_bytes(result->errors()).c_str());
			result =d->depotView->status(paths);
			Assert::IsTrue(std::string(result->output()).find("haveRev 1") != std::string::npos, L"Failed to retrieve version one");

			// Get the latest version of this file
			result =d->depotView->getLatest(paths);
			Assert::IsTrue(!result->hasErrors(),d->converter.from_bytes(result->errors()).c_str());
			result =d->depotView->status(paths);
			std::string status(result->output());
			auto beginHead = status.find("headRev ");
			auto beginHave = status.find("haveRev ");
			Assert::IsTrue(beginHead != std::string::npos, L"Cannot determine head revision");
			Assert::IsTrue(beginHave != std::string::npos, L"Cannot determine have revision");
			beginHead += 8;
			beginHave += 8;
			auto headRev = status.substr(beginHead, status.find_first_not_of("0123456789", beginHead) - beginHead);
			auto haveRev = status.substr(beginHave, status.find_first_not_of("0123456789", beginHave) - beginHave);
			Assert::IsTrue(headRev == haveRev);
		}

		TEST_METHOD(RenameFile)
		{
			IDepotView::PathList paths;
			paths.emplace_back(d->thisFile);
			auto result =d->depotView->status(paths);
			Assert::IsTrue(!result->hasErrors(),d->converter.from_bytes(result->errors()).c_str());

			Assert::IsTrue(std::string(result->output()).find("action") == std::string::npos, L"Can't complete test, file already checked out");

			// Create an empty changelist
			IDepotView::ChangeListId changelist;
			result =d->depotView->createChangeList("Unit Test Rename", changelist);
			Assert::IsTrue(!result->hasErrors(),d->converter.from_bytes(result->errors()).c_str());

			// Rename the file 
			IDepotView::FilePairs pairs;
			pairs[d->thisFile] = d->thisFile + "@test%rename#.cpp";
			result =d->depotView->rename(pairs, changelist);
			Assert::IsTrue(!result->hasErrors(),d->converter.from_bytes(result->errors()).c_str());
			
			// Revert the file
			paths.clear();
			paths.emplace_back(pairs[d->thisFile]);
			result =d->depotView->revert(paths);
			Assert::IsTrue(!result->hasErrors(),d->converter.from_bytes(result->errors()).c_str());

			// Delete the change list
			result =d->depotView->deleteEmptyChangeList(changelist);
			Assert::IsTrue(!result->hasErrors(),d->converter.from_bytes(result->errors()).c_str());
		}

		TEST_METHOD(RemoveFile)
		{
			IDepotView::PathList paths;
			paths.emplace_back(d->thisFile);
			auto result =d->depotView->status(paths);
			Assert::IsTrue(!result->hasErrors(),d->converter.from_bytes(result->errors()).c_str());

			Assert::IsTrue(std::string(result->output()).find("action") == std::string::npos, L"Can't complete test, file already checked out");

			// Create an empty changelist
			IDepotView::ChangeListId changelist;
			result =d->depotView->createChangeList("Unit Test Remove", changelist);
			Assert::IsTrue(!result->hasErrors(),d->converter.from_bytes(result->errors()).c_str());

			// Remove the file 
			result =d->depotView->remove(paths, changelist);
			Assert::IsTrue(!result->hasErrors(),d->converter.from_bytes(result->errors()).c_str());

			// Revert the file
			result =d->depotView->revert(paths);
			Assert::IsTrue(!result->hasErrors(),d->converter.from_bytes(result->errors()).c_str());

			// Delete the change list
			result =d->depotView->deleteEmptyChangeList(changelist);
			Assert::IsTrue(!result->hasErrors(),d->converter.from_bytes(result->errors()).c_str());
		}

		TEST_METHOD(ReopenFile)
		{
			IDepotView::PathList paths;
			paths.emplace_back(d->thisFile);
			auto result =d->depotView->status(paths);
			Assert::IsTrue(!result->hasErrors(),d->converter.from_bytes(result->errors()).c_str());

			Assert::IsTrue(std::string(result->output()).find("action") == std::string::npos, L"Can't complete test, file already checked out");

			// Create an empty changelist
			IDepotView::ChangeListId changelist;
			result =d->depotView->createChangeList("Unit Test Reopen", changelist);
			Assert::IsTrue(!result->hasErrors(),d->converter.from_bytes(result->errors()).c_str());

			// Open the file in changelist
			result =d->depotView->checkout(paths, changelist);
			Assert::IsTrue(!result->hasErrors(),d->converter.from_bytes(result->errors()).c_str());

			// Reopen the file in the default changelist
			result =d->depotView->reopen(paths);
			Assert::IsTrue(!result->hasErrors(),d->converter.from_bytes(result->errors()).c_str());

			// Revert the file
			result =d->depotView->revert(paths);
			Assert::IsTrue(!result->hasErrors(),d->converter.from_bytes(result->errors()).c_str());

			// Delete the change list
			result =d->depotView->deleteEmptyChangeList(changelist);
			Assert::IsTrue(!result->hasErrors(),d->converter.from_bytes(result->errors()).c_str());
		}

		TEST_METHOD(SubmitFile)
		{
			IDepotView::PathList paths;
			paths.emplace_back(d->thisFile);
			auto result =d->depotView->status(paths);

			// If this file is not already checked out there is nothing to submit
			if(std::string(result->output()).find("action") == std::string::npos)
				return;

			// Create an empty changelist
			IDepotView::ChangeListId changelist;
			result =d->depotView->createChangeList("[NGT] [Perforce] Unit Test Submit", changelist);
			Assert::IsTrue(!result->hasErrors(),d->converter.from_bytes(result->errors()).c_str());

			// Reopen the file in changelist
			result =d->depotView->reopen(paths, changelist);
			Assert::IsTrue(!result->hasErrors(),d->converter.from_bytes(result->errors()).c_str());

			// Submit the file
			result =d->depotView->submit(changelist);
			Assert::IsTrue(!result->hasErrors(),d->converter.from_bytes(result->errors()).c_str());
		}

		TEST_METHOD(GetMultiFileStatus)
		{
			IDepotView::PathList paths;
			paths.emplace_back(FilePath(d->thisFile).getFolder() + "...");
			paths.emplace_back(d->tempFile);
			auto result =d->depotView->status(paths);
			// There should be results for all the files in this directory
			Assert::IsTrue(result->output() != nullptr && result->output() != '\0');
			// The tempFile should result in an error
			Assert::IsTrue(result->errors() != nullptr && result->errors() != '\0');
			// Verify there is a result for this file
			auto results = result->results();
			auto found = std::find_if(results.begin(), results.end(), [&](Attributes& result)
			{
				return _stricmp(result["clientFile"].c_str(), d->thisFile.c_str()) == 0;
			});
			Assert::IsTrue(found != results.end());
		}
	};
}
} // end namespace wgt
