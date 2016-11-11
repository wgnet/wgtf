#ifndef QT_FRAMEWORK_HPP
#define QT_FRAMEWORK_HPP

#include "core_automation/interfaces/automation_interface.hpp"
#include "core_dependency_system/i_interface.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_script/type_converter_queue.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_ui_framework/i_progress_dialog.hpp"
#include "core_qt_common/qt_progress_dialog.hpp"
#include <tuple>

class QUrl;
struct QMetaObject;
class QQmlComponent;
class QString;
class QIODevice;
class QQmlIncubationController;
class QFileDialog;
class QStringList;

namespace wgt
{
class IQtTypeConverter;
class QtScriptingEngine;
class IComponentContext;
class QtDefaultSpacing;
class QtGlobalSettings;
class QmlWindow;
class QtWindow;
class QtPreferences;
class ICommandManager;
class QmlComponent;
class ActionManager;

namespace QtFramework_Locals
{
class QtCommandEventListener;
}

class QtFramework : public Implements<IQtFramework>,
                    public Depends<AutomationInterface, ICommandManager, IUIApplication, IViewCreator>
{
public:
	typedef std::tuple<const unsigned char*, const unsigned char*, const unsigned char*> ResourceData;

	QtFramework(IComponentContext& contextManager);
	virtual ~QtFramework();

	void initialise(IComponentContext& contextManager);
	void finalise();

	// IQtFramework
	QQmlEngine* qmlEngine() const override;
	QFileSystemWatcher* qmlWatcher() const override;
	virtual void setIncubationTime(int msecs) override;
	virtual void incubate() override;
	const QtPalette* palette() const override;
	QtGlobalSettings* qtGlobalSettings() const override;
	void addImportPath(const QString& path);

	void registerTypeConverter(IQtTypeConverter& converter) override;
	bool registerResourceData(const unsigned char* qrc_struct, const unsigned char* qrc_name,
	                          const unsigned char* qrc_data) override;
	void deregisterTypeConverter(IQtTypeConverter& converter) override;
	QVariant toQVariant(const Variant& variant, QObject* parent) const override;
	Variant toVariant(const QVariant& qVariant) const override;

	QQmlComponent* toQmlComponent(IComponent& component) override;
	QWidget* toQWidget(IView& view) override;
	void retainQWidget(IView& view) override;

	std::unique_ptr<IAction> createAction(const char* id, std::function<void(IAction*)> func,
	                                      std::function<bool(const IAction*)> enableFunc,
	                                      std::function<bool(const IAction*)> checkedFunc) override;
	std::unique_ptr<IAction> createAction(const char* id, const char* text, const char* path,
	                                      std::function<void(IAction*)> func,
	                                      std::function<bool(const IAction*)> enableFunc,
	                                      std::function<bool(const IAction*)> checkedFunc, int actionOrder) override;
	std::unique_ptr<IComponent> createComponent(const char* resource, ResourceType type) override;

	std::unique_ptr<IView> createView(const char* resource, ResourceType type, const Variant& context) override;
	std::unique_ptr<IView> createView(const char* uniqueName, const char* resource, ResourceType type,
	                                  const Variant& context) override;
	std::unique_ptr<IWindow> createWindow(const char* resource, ResourceType type, const Variant& context) override;

	wg_future<std::unique_ptr<IView>> createViewAsync(const char* uniqueName, const char* resource, ResourceType type,
	                                                  const Variant& context,
	                                                  std::function<void(IView&)> loadedHandler) override;
	void createWindowAsync(const char* resource, ResourceType type, const Variant& context,
	                       std::function<void(std::unique_ptr<IWindow>&)> loadedHandler) override;

	/*!
	    Creates a progress dialog with the specified title, label, and maximum progress
	    The dialog will only display if duration passes.
	    If cancelText is empty the progress can't be canceled, otherwise it is used for the cancel button
	*/
	virtual IProgressDialogPtr createProgressDialog(const std::string& title, const std::string& label,
	                                                const std::string& cancelText, uint32_t minimum, uint32_t maximum,
	                                                std::chrono::milliseconds duration) override;

	/*!
	    Creates a modeless progress dialog with the specified title, label, and maximum progress
	    The dialog will only display if duration passes.
	    If cancelText is empty the progress can't be canceled, otherwise it is used for the cancel button
	    Each update the callback will be called so work can be done and progress made
	*/
	virtual IProgressDialog* createModelessProgressDialog(const std::string& title, const std::string& label,
	                                                      const std::string& cancelText, uint32_t minimum,
	                                                      uint32_t maximum, std::chrono::milliseconds duration,
	                                                      ModelessProgressCallback callback) override;

	/*!
	Creates a indeterminate modal progress dialog with the specified title and label.
	*/
	virtual IProgressDialogPtr createIndeterminateProgressDialog(const std::string& title,
	                                                             const std::string& label) override;

	/*!
	Shows an open file dialog with default options. Caption for the dialog, working directory and file
	filter are passed in. Returns an existing file path selected by user.
	*/
	virtual const std::vector<std::string> showOpenFileDialog(
	const std::string& caption, const std::string& directory, const std::string& filter,
	const QtFileDialogOptions& options = (QtFileDialogOptions)(QtFileDialogOptions::ShowDirsOnly |
	                                                           QtFileDialogOptions::DontResolveSymlinks)) override;

	/*!
	Shows a save file dialog with default options. Caption for the dialog, working directory and file
	filter are passed in. Returns the file path with the new filename.
	*/
	virtual const std::string showSaveAsFileDialog(const std::string& caption, const std::string& directory,
	                                               const std::string& filter,
	                                               const QtFileDialogOptions& options) override;

	/*!
	Shows a file directory dialog with default options. Caption for the dialog, working directory are passed in.
	Return an existing directory selected by user.
	*/
	virtual const std::string showSelectDirectoryDialog(const std::string& caption, const std::string& directory,
	                                                    const QtFileDialogOptions& options) override;

	void enableAsynchronousViewCreation(bool enabled) override;
	void loadActionData(const char* resource, ResourceType type) override;
	void registerComponent(const char* id, const char* version, IComponent& component) override;
	void registerComponentProvider(IComponentProvider& provider) override;
	IComponent* findComponent(const TypeId& typeId, std::function<bool(const ItemRole::Id&)>& predicate,
	                          const char* version) const override;

	virtual void registerDialog(const char* id, const char* version, std::shared_ptr<IDialog> dialog) override;
	virtual std::shared_ptr<IDialog> findDialog(const char* id, const char* version) override;

	virtual void setPluginPath(const std::string& path) override;
	virtual const std::string& getPluginPath() const override;

	int displayMessageBox(const char* title, const char* message, int buttons) override;

	IPreferences* getPreferences() override;
	void doOnUIThread(std::function<void()>) override;

	virtual void registerQmlType(ObjectHandle type) override;

	virtual void showShortcutConfig() const override;

	virtual std::unique_ptr<IDialog> createDialog(const char* resource,
	                                              ObjectHandleT<DialogModel> model = nullptr) override;

	virtual void showDialog(const char* resource, const IDialog::Mode mode = IDialog::Mode::MODAL,
	                        const IDialog::ClosedCallback& callback = nullptr) override;

	virtual void showDialog(const char* resource, ObjectHandleT<DialogModel> model,
	                        const IDialog::Mode mode = IDialog::Mode::MODAL,
	                        const IDialog::ClosedCallback& callback = nullptr) override;

	virtual bool getWGCopyableEnableStatus() const override;
	virtual void setWGCopyableEnableStatus(bool enabled) override;

protected:
	virtual QmlWindow* createQmlWindow();
	virtual QtWindow* createQtWindow(QIODevice& source);

private:
	QmlComponent* createComponent(const QUrl& resource);
	wg_future<std::unique_ptr<IView>> createViewInternal(const char* uniqueName, const char* resource,
	                                                     ResourceType type, const Variant& context,
	                                                     std::function<void(IView&)> loadedHandler, bool async);
	void createWindowInternal(const char* resource, ResourceType type, const Variant& context,
	                          std::function<void(std::unique_ptr<IWindow>&)> loadedHandler, bool async);

	void findReloadableFiles(const QString& path, const QStringList& filter, QStringList& files);
	void registerReloadableFiles();
	void registerDefaultComponents();
	void registerDefaultComponentProviders();
	void registerDefaultDialogs();
	void registerDefaultTypeConverters();
	void unregisterResources();
	void onApplicationStartUp();

	std::unique_ptr<QQmlEngine> qmlEngine_;
	std::unique_ptr<QFileSystemWatcher> qmlWatcher_;
	std::unique_ptr<QtScriptingEngine> scriptingEngine_;
	std::unique_ptr<QtPalette> palette_;
	std::unique_ptr<QtDefaultSpacing> defaultQmlSpacing_;
	std::unique_ptr<QtGlobalSettings> globalQmlSettings_;
	std::vector<std::unique_ptr<IComponent>> defaultComponents_;
	std::vector<std::unique_ptr<IComponentProvider>> defaultComponentProviders_;
	std::vector<std::unique_ptr<IQtTypeConverter>> defaultTypeConverters_;

	struct ComponentVersion
	{
		static std::vector<int> tokenise(const char* version);
		bool operator()(const std::vector<int>& a, const std::vector<int>& b) const;
	};
	std::map<std::string, std::map<std::vector<int>, IComponent*, ComponentVersion>> components_;
	std::vector<IComponentProvider*> componentProviders_;
	std::vector<ResourceData> registeredResources_;

	std::vector<std::shared_ptr<IDialog>> defaultDialogs_;
	std::map<std::string, std::map<std::vector<int>, std::weak_ptr<IDialog>, ComponentVersion>> dialogs_;

	QMetaObject* constructMetaObject(QMetaObject* original);
	std::vector<std::unique_ptr<QMetaObject>> registeredTypes_;

	typedef TypeConverterQueue<IQtTypeConverter, QVariant> QtTypeConverters;
	QtTypeConverters typeConverters_;

	typedef std::tuple<std::unique_ptr<IDialog>, bool, Connection> TemporaryDialogData;
	std::vector<TemporaryDialogData> temporaryDialogs_;
	std::mutex temporaryDialogMutex_;

	std::string pluginPath_;
	std::unique_ptr<ActionManager> actionManager_;
	std::unique_ptr<QtFramework_Locals::QtCommandEventListener> commandEventListener_;
	IComponentContext& context_;
	std::unique_ptr<QObject> worker_;
	std::unique_ptr<IWindow> shortcutDialog_;
	ConnectionHolder connections_;
	std::atomic<int> incubationTime_;
	std::unique_ptr<QQmlIncubationController> incubationController_;
	IInterface* preferences_;

	bool useAsyncViewLoading_;
};
} // end namespace wgt
#endif
