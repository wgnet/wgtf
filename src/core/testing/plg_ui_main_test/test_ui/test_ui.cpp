#include "test_ui.hpp"
#include "test_viewport.hpp"
#include "context.hpp"
#include "core_logging/logging.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "interfaces/i_datasource.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_window.hpp"
#include "core_logging/logging.hpp"

namespace wgt
{
TestUI::TestUI(IEnvManager& envManager) 
    : EnvComponentT(envManager)
{
}

TestUI::~TestUI()
{
}

const char* TestUI::getEnvComponentId() const
{
	static const char* s_id = "testui_envcomponentid";
	return s_id;
}

void TestUI::init()
{
	initEnvComponent();

	auto uiFramework = get<IUIFramework>();
	auto uiApplication = get<IUIApplication>();
    auto viewCreator = get<IViewCreator>();
    auto dataSrcMngr = get<IDataSourceManager>();

	uiFramework->loadActionData(":/testing_ui_main/actions.xml", IUIFramework::ResourceType::File);

	// hook open/close
	testOpen_ = uiFramework->createAction("Open", std::bind(&TestUI::open, this), std::bind(&TestUI::canOpen, this));
	testClose_ = uiFramework->createAction("Close", std::bind(&TestUI::close, this), std::bind(&TestUI::canClose, this));

	uiApplication->addAction(*testOpen_);
	uiApplication->addAction(*testClose_);

	context_ = ManagedObject<TestUIContext>::make();
	context_->initialise(dataSrcMngr->openDataSource()->getTestPage());
	view_ = viewCreator->createView("testing_ui_main/test_property_tree_panel.qml", context_.getHandleT());
}

void TestUI::fini()
{
	finiEnvComponent();

	closeAll();

	auto app = get<IUIApplication>();
	assert(app != nullptr);

	app->removeAction(*testOpen_);
	app->removeAction(*testClose_);

	testOpen_.reset();
	testClose_.reset();

	if (view_.valid())
	{
		auto view = view_.get();
		app->removeView(*view);
		view = nullptr;
	}

    context_ = nullptr;
}

void TestUI::closeAll()
{
	while (!dataSrcEnvPairs_.empty())
	{
		close();
	}
}

void TestUI::onPostEnvironmentChanged(const EnvironmentId& oldId, const EnvironmentId& newId)
{
    assert(context_ != nullptr);
	context_->select(newId);
}

void TestUI::open()
{
	assert(dataSrcEnvPairs_.size() < 5);

	auto dataSrcMngr = get<IDataSourceManager>();
	auto defManager = get<IDefinitionManager>();
	assert(defManager != nullptr);
    assert(dataSrcMngr != nullptr);

	IDataSource* dataSrc = dataSrcMngr->openDataSource();

	auto viewport = std::make_unique<TestViewport>(dataSrc->description());
	auto envIdx = viewport->getId();
	viewports_[envIdx] = std::move(viewport);

    assert(context_ != nullptr);
    context_->open(envIdx, dataSrc->getTestPage());

	dataSrcEnvPairs_.push_back(DataSrcEnvPairs::value_type(dataSrc, envIdx));

	get<IEnvManager>()->switchEnvironment(envIdx);
}

void TestUI::close()
{
	assert(dataSrcEnvPairs_.size() > 0);

	IDataSource* dataSrc = dataSrcEnvPairs_.back().first;
	const std::string envIdx = dataSrcEnvPairs_.back().second;
	dataSrcEnvPairs_.pop_back();

	assert(context_ != nullptr);
    context_->close(envIdx);

	assert(viewports_.find(envIdx) != viewports_.end());
	viewports_.erase(envIdx);

	auto dataSrcMngr = get<IDataSourceManager>();
	dataSrcMngr->closeDataSource(dataSrc);
}

bool TestUI::canOpen() const
{
	return dataSrcEnvPairs_.size() < 5;
}

bool TestUI::canClose() const
{
	return dataSrcEnvPairs_.size() > 0;
}
} // end namespace wgt
