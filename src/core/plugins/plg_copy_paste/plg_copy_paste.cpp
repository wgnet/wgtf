#include "core_generic_plugin/generic_plugin.hpp"
#include "core_variant/variant.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_copy_paste/i_copy_paste_manager.hpp"

WGT_INIT_QRC_RESOURCE

namespace wgt
{
/**
* A plugin which queries the ICopyPasteManager interface to
* create an Edit option in the menu bar with copy and paste options.
*
* @ingroup plugins
* @ingroup coreplugins
* @image html plg_copy_paste.png
* @note Requires Plugins:
*       - @ref coreplugins
*/
class CopyPastePlugin : public PluginMain
{
private:
	ICopyPasteManager* copyPasteManager_;
	IQtFramework* qtFramework;
	std::vector<IInterface*> types_;
	std::unique_ptr<IAction> toggleCopyControl_;
	std::unique_ptr<IAction> copy_;
	std::unique_ptr<IAction> paste_;

	void createCopyPasteUI(IComponentContext& contextManager)
	{
		qtFramework = contextManager.queryInterface<IQtFramework>();
		auto uiFramework = contextManager.queryInterface<IUIFramework>();
		auto uiApplication = contextManager.queryInterface<IUIApplication>();
		assert((uiFramework != nullptr) && (uiApplication != nullptr));
		uiFramework->loadActionData(":/WGCopyableFunctions/actions.xml", IUIFramework::ResourceType::File);

		using namespace std::placeholders;

		toggleCopyControl_ = uiFramework->createAction(
		"ToggleCopyControls", std::bind(&CopyPastePlugin::toggleCopyControl, this, _1),
		[](const IAction*) { return true; }, std::bind(&CopyPastePlugin::isCopyControlChecked, this, _1));

		copyPasteManager_ = contextManager.queryInterface<ICopyPasteManager>();
		assert(copyPasteManager_ != nullptr);

		copy_ = uiFramework->createAction("Copy", std::bind(&CopyPastePlugin::copy, this, _1),
		                                  std::bind(&CopyPastePlugin::canCopy, this, _1));

		paste_ = uiFramework->createAction("Paste", std::bind(&CopyPastePlugin::paste, this, _1),
		                                   std::bind(&CopyPastePlugin::canPaste, this, _1));

		uiApplication->addAction(*toggleCopyControl_);
		uiApplication->addAction(*copy_);
		uiApplication->addAction(*paste_);
	}

	void destroyActions()
	{
		paste_.reset();
		copy_.reset();
		toggleCopyControl_.reset();
	}

	void toggleCopyControl(const IAction* action)
	{
		assert(qtFramework != nullptr);
		bool enabled = qtFramework->getWGCopyableEnableStatus();
		qtFramework->setWGCopyableEnableStatus(!enabled);
	}

	bool isCopyControlChecked(const IAction* action) const
	{
		assert(qtFramework != nullptr);
		return qtFramework->getWGCopyableEnableStatus();
	}

	void copy(IAction* action)
	{
		copyPasteManager_->copy();
	}

	void paste(IAction* action)
	{
		copyPasteManager_->paste();
	}

	bool canCopy(const IAction* action) const
	{
		return isCopyControlChecked(nullptr) && copyPasteManager_->canCopy();
	}

	bool canPaste(const IAction* action) const
	{
		return isCopyControlChecked(nullptr) && copyPasteManager_->canPaste();
	}

public:
	//==========================================================================
	CopyPastePlugin(IComponentContext& contextManager) : copyPasteManager_(nullptr), qtFramework(nullptr)
	{
	}

	//==========================================================================
	bool PostLoad(IComponentContext& contextManager) override
	{
		return true;
	}

	//==========================================================================
	void Initialise(IComponentContext& contextManager) override
	{
		createCopyPasteUI(contextManager);
	}

	//==========================================================================
	bool Finalise(IComponentContext& contextManager) override
	{
		auto uiApplication = contextManager.queryInterface<IUIApplication>();
		assert(uiApplication != nullptr);
		uiApplication->removeAction(*toggleCopyControl_);
		uiApplication->removeAction(*copy_);
		uiApplication->removeAction(*paste_);
		destroyActions();
		return true;
	}

	//==========================================================================
	void Unload(IComponentContext& contextManager) override
	{
		for (auto type : types_)
		{
			contextManager.deregisterInterface(type);
		}
	}
};

PLG_CALLBACK_FUNC(CopyPastePlugin)
} // end namespace wgt
