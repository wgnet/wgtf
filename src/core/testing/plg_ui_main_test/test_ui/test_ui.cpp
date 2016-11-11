#include "test_ui.hpp"
#include "context.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_command_system/i_env_system.hpp"
#include "core_logging/logging.hpp"
#include "core_reflection/interfaces/i_reflection_property_setter.hpp"
#include "core_reflection/interfaces/i_reflection_controller.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "interfaces/i_datasource.hpp"

#include "core_data_model/reflection_proto/property_tree_model.hpp"

#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/i_window.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include "core_serialization/i_file_system.hpp"
#include "core_serialization/serializer/xml_serializer.hpp"
#include "core_copy_paste/i_copy_paste_manager.hpp"
#include "core_logging/logging.hpp"

namespace wgt
{
namespace
{
static const char* s_historyVersion = "_ui_main_ver_1_0_15";
typedef XMLSerializer HistorySerializer;
}
//==============================================================================
TestUI::TestUI(IComponentContext& context) : Depends(context), context_(context)
{
}

//==============================================================================
TestUI::~TestUI()
{
}

//==============================================================================
void TestUI::init(IUIApplication& uiApplication, IUIFramework& uiFramework)
{
	app_ = &uiApplication;
	fw_ = &uiFramework;
	uiFramework.loadActionData(":/testing_ui_main/actions.xml", IUIFramework::ResourceType::File);

	createActions(uiFramework);
	addActions(uiApplication);
}

//------------------------------------------------------------------------------
void TestUI::fini()
{
	closeAll();
	destroyActions();
}

// =============================================================================
void TestUI::createActions(IUIFramework& uiFramework)
{
	// hook open/close
	testOpen_ = uiFramework.createAction("Open", std::bind(&TestUI::open, this), std::bind(&TestUI::canOpen, this));

	testClose_ = uiFramework.createAction("Close", std::bind(&TestUI::close, this), std::bind(&TestUI::canClose, this));

	ICommandManager* commandSystemProvider = get<ICommandManager>();
	assert(commandSystemProvider);
	if (commandSystemProvider == NULL)
	{
		return;
	}
}

// =============================================================================
void TestUI::createViews(IUIFramework& uiFramework, IDataSource* dataSrc, int envIdx)
{
	assert(app_ != nullptr);
	auto defManager = get<IDefinitionManager>();
	assert(defManager != nullptr);
	auto controller = get<IReflectionController>();
	assert(controller != nullptr);

	const bool managed = true;
	auto contextObject = defManager->create<TestUIContext>(managed);
	assert(contextObject != nullptr);
	contextObject->initialize(
	std::unique_ptr<AbstractTreeModel>(new proto::PropertyTreeModel(context_, dataSrc->getTestPage())));
	test1Contexts_.emplace_back(contextObject);
	std::string uniqueName1 = dataSrc->description() + std::string("testing_ui_main/test_property_tree_panel.qml");
	auto viewAsync = uiFramework.createViewAsync(uniqueName1.c_str(), "testing_ui_main/test_property_tree_panel.qml",
	                                             IUIFramework::ResourceType::Url, test1Contexts_.back());
	test1Views_.emplace_back(TestViews::value_type(std::move(viewAsync.get()), envIdx));
	auto view = test1Views_.back().first.get();
	view->registerListener(this);
	app_->addView(*view);
}

// =============================================================================
void TestUI::destroyActions()
{
	assert(app_ != nullptr);
	app_->removeAction(*testOpen_);
	app_->removeAction(*testClose_);
	testOpen_.reset();
	testClose_.reset();
}

// =============================================================================
void TestUI::destroyViews(size_t idx)
{
	removeViews(idx);
	test1Contexts_.erase(test1Contexts_.begin() + idx);
	test1Views_.erase(test1Views_.begin() + idx);
}

// =============================================================================
void TestUI::closeAll()
{
	while (!dataSrcEnvPairs_.empty())
	{
		close();
	}

	assert(test1Views_.empty());
}

// =============================================================================
void TestUI::addActions(IUIApplication& uiApplication)
{
	uiApplication.addAction(*testOpen_);
	uiApplication.addAction(*testClose_);
}

// =============================================================================
void TestUI::removeViews(size_t idx)
{
	assert(app_ != nullptr);
	if (test1Views_[idx].first != nullptr)
	{
		app_->removeView(*test1Views_[idx].first);
	}
}

void TestUI::onFocusIn(IView* view)
{
	// NGT_MSG("%s focus in\n", view->title());

	auto pr = [&](TestViews::value_type& x) { return x.first.get() == view; };

	auto it1 = std::find_if(test1Views_.begin(), test1Views_.end(), pr);
	int envId = -1;
	if (it1 != test1Views_.end())
	{
		envId = it1->second;
	}
	assert(envId != -1);
	get<IEnvManager>()->selectEnv(envId);
	auto findIt = historyFlags_.find(envId);
	assert(findIt != historyFlags_.end());
	if (!findIt->second)
	{
		auto pre = [envId](DataSrcEnvPairs::value_type& x) { return x.second == envId; };
		auto it = std::find_if(dataSrcEnvPairs_.begin(), dataSrcEnvPairs_.end(), pre);
		assert(it != dataSrcEnvPairs_.end());
		auto dataSrc = it->first;
		assert(dataSrc != nullptr);
		std::string file = dataSrc->description();
		file += s_historyVersion;
		auto fileSystem = get<IFileSystem>();
		assert(fileSystem != nullptr);
		if (fileSystem->exists(file.c_str()))
		{
			auto defManager = get<IDefinitionManager>();
			assert(defManager != nullptr);
			IFileSystem::IStreamPtr fileStream = fileSystem->readFile(file.c_str(), std::ios::in | std::ios::binary);
			HistorySerializer serializer(*fileStream, *defManager);
			std::string version;
			serializer.deserialize(version);
			if (version == s_historyVersion)
			{
				auto cmdMgr = get<ICommandManager>();
				assert(cmdMgr != nullptr);
				cmdMgr->LoadHistory(serializer);
			}
		}
		historyFlags_[envId] = true;
	}
}

void TestUI::onFocusOut(IView* view)
{
	// NGT_MSG("%s focus out\n", view->title());
}

void TestUI::open()
{
	assert(test1Views_.size() < 5);

	IDataSourceManager* dataSrcMngr = get<IDataSourceManager>();
	IDataSource* dataSrc = dataSrcMngr->openDataSource();

	IEnvManager* em = get<IEnvManager>();
	int envIdx = em->addEnv(dataSrc->description());
	auto findIt = historyFlags_.find(envIdx);
	assert(findIt == historyFlags_.end());
	historyFlags_[envIdx] = false;

	dataSrcEnvPairs_.push_back(DataSrcEnvPairs::value_type(dataSrc, envIdx));
	createViews(*fw_, dataSrc, envIdx);
}

void TestUI::close()
{
	assert(dataSrcEnvPairs_.size() > 0);

	IDataSource* dataSrc = dataSrcEnvPairs_.back().first;
	int envIdx = dataSrcEnvPairs_.back().second;
	auto findIt = historyFlags_.find(envIdx);
	assert(findIt != historyFlags_.end());
	historyFlags_.erase(envIdx);
	dataSrcEnvPairs_.pop_back();
	destroyViews(dataSrcEnvPairs_.size());

	auto cmdMgr = get<ICommandManager>();
	auto defManager = get<IDefinitionManager>();
	assert(cmdMgr != nullptr && defManager != nullptr);
	ResizingMemoryStream stream;
	HistorySerializer serializer(stream, *defManager);
	serializer.serialize(s_historyVersion);
	cmdMgr->SaveHistory(serializer);
	std::string file = dataSrc->description();
	file += s_historyVersion;
	auto fileSystem = get<IFileSystem>();
	assert(fileSystem != nullptr);
	fileSystem->writeFile(file.c_str(), stream.buffer().c_str(), stream.buffer().size(),
	                      std::ios::out | std::ios::binary);

	IEnvManager* em = get<IEnvManager>();
	em->removeEnv(envIdx);

	auto dataSrcMngr = get<IDataSourceManager>();
	dataSrcMngr->closeDataSource(dataSrc);
}

bool TestUI::canOpen() const
{
	return test1Views_.size() < 5;
}

bool TestUI::canClose() const
{
	return test1Views_.size() > 0;
}
} // end namespace wgt
