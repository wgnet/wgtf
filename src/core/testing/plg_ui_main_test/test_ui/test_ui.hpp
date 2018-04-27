#ifndef TEST_UI_H
#define TEST_UI_H

#include <memory>
#include "core_dependency_system/depends.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_common/wg_future.hpp"
#include <unordered_map>
#include "core_environment_system/i_env_system.hpp"
#include "core_object/managed_object.hpp"

namespace wgt
{
class IAction;
class IUIApplication;
class IUIFramework;
class IWindow;
class IDataSource;
class IDataSourceManager;
class ICommandManager;
class IDefinitionManager;
class IEnvManager;
class IFileSystem;
class ObjectHandle;
class IViewCreator;
class IViewport;
class TestUIContext;

class TestUI : EnvComponentT<IEnvComponentState>,
               Depends<IDefinitionManager, ICommandManager, IDataSourceManager, IEnvManager, IFileSystem, IUIFramework,
                       IUIApplication, IViewCreator>
{
public:
	TestUI(IEnvManager& envManager);
	~TestUI();

	void init();
	void fini();

private:
	void undo(IAction* action);
	void redo(IAction* action);
	bool canUndo(const IAction* action) const;
	bool canRedo(const IAction* action) const;
	void open();
	void close();
	void closeAll();
	bool canOpen() const;
	bool canClose() const;

	virtual const char* getEnvComponentId() const override;
	virtual void onPostEnvironmentChanged(const EnvironmentId& oldId, const EnvironmentId& newId) override;

	wg_future<std::unique_ptr<IView>> view_;
    ManagedObject<TestUIContext> context_;
	std::unique_ptr<IAction> testOpen_;
	std::unique_ptr<IAction> testClose_;
	std::unordered_map<std::string, std::unique_ptr<IViewport>> viewports_;

	typedef std::vector<std::pair<IDataSource*, std::string>> DataSrcEnvPairs;
	DataSrcEnvPairs dataSrcEnvPairs_;
};

} // end namespace wgt
#endif // TEST_UI_H
