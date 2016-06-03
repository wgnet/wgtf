#include "stdafx.h"
#include "CppUnitTest.h"
#include "generic_plugin_manager/generic_plugin_manager.hpp"
#include "generic_plugin/interfaces/i_context_manager.hpp"
#include "generic_plugin/interfaces/i_plugin_context_manager.hpp"

#include "interfaces/version_control/i_version_control.hpp"
#include "interfaces/version_control/i_depot_view.hpp"

#include <locale>
#include <codecvt>
#include <string>

namespace wgt
{
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
EXTERN_C IMAGE_DOS_HEADER __ImageBase;

namespace UnitTests
{		
	TEST_CLASS(PerforceUnitTests)
	{
		GenericPluginManager pluginManager;
		std::string thisFile;
		std::string tempFile;
		IVersionControl* versionControl;
		IDepotViewPtr depotView;
		typedef std::codecvt<wchar_t, char, std::mbstate_t> converter_type;
		std::wstring_convert<converter_type> converter;

	public:
		PerforceUnitTests() : thisFile(__FILE__)
		{
			auto dir = thisFile.substr(0, thisFile.rfind('\\'));
			char tempFilePath[MAX_PATH] = { 0 };
			::GetTempFileNameA(dir.c_str(), "P4T", 0, tempFilePath);
			tempFile = tempFilePath;

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

		~PerforceUnitTests()
		{
			::DeleteFileA(tempFile.c_str());
		}

		TEST_METHOD(CreateChangeList)
		{
			// Create an empty changelist
			IDepotView::ChangeListId changelist;
			auto result = depotView->createChangeList("Multi\rLine\nTest\r\nChangelist", changelist);
			Assert::IsTrue(!result->hasErrors(), converter.from_bytes(result->errors()).c_str());

			// Delete the change list
			result = depotView->deleteEmptyChangeList(changelist);
			Assert::IsTrue(!result->hasErrors(), converter.from_bytes(result->errors()).c_str());
		}

		TEST_METHOD(AddAndRevertFile)
		{
			// Create an empty changelist
			IDepotView::ChangeListId changelist;
			auto result = depotView->createChangeList("Multi\rLine\nTest\r\nChangelist", changelist);
			Assert::IsTrue(!result->hasErrors(), converter.from_bytes(result->errors()).c_str());

			// Add the temp file to the newly created change list
			IDepotView::PathList paths;
			paths.emplace_back(tempFile);
			result = depotView->add(paths, changelist);
			Assert::IsTrue(!result->hasErrors(), converter.from_bytes(result->errors()).c_str());

			// Revert the file
			result = depotView->revert(paths);
			Assert::IsTrue(!result->hasErrors(), converter.from_bytes(result->errors()).c_str());

			// Delete the change list
			result = depotView->deleteEmptyChangeList(changelist);
			Assert::IsTrue(!result->hasErrors(), converter.from_bytes(result->errors()).c_str());
		}

		TEST_METHOD(CheckoutFile)
		{
			IDepotView::PathList paths;
			paths.emplace_back(thisFile);
			auto result = depotView->status(paths);
			Assert::IsTrue(!result->hasErrors(), converter.from_bytes(result->errors()).c_str());

			Assert::IsTrue(std::string(result->output()).find("action") == std::string::npos, L"Can't complete test, file already checked out");
			
			// Create an empty changelist
			IDepotView::ChangeListId changelist;
			result = depotView->createChangeList("Unit Test Checkout", changelist);
			Assert::IsTrue(!result->hasErrors(), converter.from_bytes(result->errors()).c_str());

			// Checkout the file
			result = depotView->checkout(paths, changelist);
			Assert::IsTrue(!result->hasErrors(), converter.from_bytes(result->errors()).c_str());
			Assert::IsTrue(std::string(result->output()).find("action edit") != std::string::npos);

			// Revert the file
			result = depotView->revert(paths);
			Assert::IsTrue(!result->hasErrors(), converter.from_bytes(result->errors()).c_str());

			// Delete the change list
			result = depotView->deleteEmptyChangeList(changelist);
			Assert::IsTrue(!result->hasErrors(), converter.from_bytes(result->errors()).c_str());
		}

		TEST_METHOD(GetFileRevisions)
		{
			IDepotView::PathList paths;
			paths.emplace_back(thisFile);
			auto result = depotView->status(paths);
			Assert::IsTrue(!result->hasErrors(), converter.from_bytes(result->errors()).c_str());

			Assert::IsTrue(std::string(result->output()).find("action") == std::string::npos, L"Can't complete test, file already checked out");

			// Get the first version of this file
			result = depotView->get(paths, 1);
			Assert::IsTrue(!result->hasErrors(), converter.from_bytes(result->errors()).c_str());
			result = depotView->status(paths);
			Assert::IsTrue(std::string(result->output()).find("haveRev 1") != std::string::npos, L"Failed to retrieve version one");

			// Get the latest version of this file
			result = depotView->getLatest(paths);
			Assert::IsTrue(!result->hasErrors(), converter.from_bytes(result->errors()).c_str());
			result = depotView->status(paths);
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
			paths.emplace_back(thisFile);
			auto result = depotView->status(paths);
			Assert::IsTrue(!result->hasErrors(), converter.from_bytes(result->errors()).c_str());

			Assert::IsTrue(std::string(result->output()).find("action") == std::string::npos, L"Can't complete test, file already checked out");

			// Create an empty changelist
			IDepotView::ChangeListId changelist;
			result = depotView->createChangeList("Unit Test Rename", changelist);
			Assert::IsTrue(!result->hasErrors(), converter.from_bytes(result->errors()).c_str());

			// Rename the file 
			IDepotView::FilePairs pairs;
			pairs[thisFile] = thisFile + "@test%rename#.cpp";
			result = depotView->rename(pairs, changelist);
			Assert::IsTrue(!result->hasErrors(), converter.from_bytes(result->errors()).c_str());
			
			// Revert the file
			paths.clear();
			paths.emplace_back(pairs[thisFile]);
			result = depotView->revert(paths);
			Assert::IsTrue(!result->hasErrors(), converter.from_bytes(result->errors()).c_str());

			// Delete the change list
			result = depotView->deleteEmptyChangeList(changelist);
			Assert::IsTrue(!result->hasErrors(), converter.from_bytes(result->errors()).c_str());
		}

		TEST_METHOD(RemoveFile)
		{
			IDepotView::PathList paths;
			paths.emplace_back(thisFile);
			auto result = depotView->status(paths);
			Assert::IsTrue(!result->hasErrors(), converter.from_bytes(result->errors()).c_str());

			Assert::IsTrue(std::string(result->output()).find("action") == std::string::npos, L"Can't complete test, file already checked out");

			// Create an empty changelist
			IDepotView::ChangeListId changelist;
			result = depotView->createChangeList("Unit Test Remove", changelist);
			Assert::IsTrue(!result->hasErrors(), converter.from_bytes(result->errors()).c_str());

			// Remove the file 
			result = depotView->remove(paths, changelist);
			Assert::IsTrue(!result->hasErrors(), converter.from_bytes(result->errors()).c_str());

			// Revert the file
			result = depotView->revert(paths);
			Assert::IsTrue(!result->hasErrors(), converter.from_bytes(result->errors()).c_str());

			// Delete the change list
			result = depotView->deleteEmptyChangeList(changelist);
			Assert::IsTrue(!result->hasErrors(), converter.from_bytes(result->errors()).c_str());
		}

		TEST_METHOD(ReopenFile)
		{
			IDepotView::PathList paths;
			paths.emplace_back(thisFile);
			auto result = depotView->status(paths);
			Assert::IsTrue(!result->hasErrors(), converter.from_bytes(result->errors()).c_str());

			Assert::IsTrue(std::string(result->output()).find("action") == std::string::npos, L"Can't complete test, file already checked out");

			// Create an empty changelist
			IDepotView::ChangeListId changelist;
			result = depotView->createChangeList("Unit Test Reopen", changelist);
			Assert::IsTrue(!result->hasErrors(), converter.from_bytes(result->errors()).c_str());

			// Open the file in changelist
			result = depotView->checkout(paths, changelist);
			Assert::IsTrue(!result->hasErrors(), converter.from_bytes(result->errors()).c_str());

			// Reopen the file in the default changelist
			result = depotView->reopen(paths);
			Assert::IsTrue(!result->hasErrors(), converter.from_bytes(result->errors()).c_str());

			// Revert the file
			result = depotView->revert(paths);
			Assert::IsTrue(!result->hasErrors(), converter.from_bytes(result->errors()).c_str());

			// Delete the change list
			result = depotView->deleteEmptyChangeList(changelist);
			Assert::IsTrue(!result->hasErrors(), converter.from_bytes(result->errors()).c_str());
		}

		TEST_METHOD(SubmitFile)
		{
			IDepotView::PathList paths;
			paths.emplace_back(thisFile);
			auto result = depotView->status(paths);

			// If this file is not already checked out there is nothing to submit
			if(std::string(result->output()).find("action") == std::string::npos)
				return;

			// Create an empty changelist
			IDepotView::ChangeListId changelist;
			result = depotView->createChangeList("[NGT] [Perforce] Unit Test Submit", changelist);
			Assert::IsTrue(!result->hasErrors(), converter.from_bytes(result->errors()).c_str());

			// Reopen the file in changelist
			result = depotView->reopen(paths, changelist);
			Assert::IsTrue(!result->hasErrors(), converter.from_bytes(result->errors()).c_str());

			// Submit the file
			result = depotView->submit(changelist);
			Assert::IsTrue(!result->hasErrors(), converter.from_bytes(result->errors()).c_str());
		}
	};
}
} // end namespace wgt
