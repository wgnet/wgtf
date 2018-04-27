#ifndef QT_FRAMEWORK_HPP
#define QT_FRAMEWORK_HPP

#include "core_qt_common/i_qt_framework.hpp"
#include "core_qt_common/qt_progress_dialog.hpp"
#include "core_qt_common/private/component_version.hpp"
#include "core_dependency_system/i_interface.hpp"
#include "core_script/type_converter_queue.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_ui_framework/i_progress_dialog.hpp"
#include <tuple>
#include <unordered_map>

struct QMetaObject;
class QQmlComponent;
class QString;
class QIODevice;
class QQmlIncubationController;
class QFileDialog;
class QStringList;
class QSize;
class QImage;

namespace wgt
{
class IComponentContext;
class QtDefaultSpacing;
class QtGlobalSettings;
class QmlWindow;
class QtWindow;
class QtPreferences;
class ICommandManager;
class ActionManager;
class IPluginContextManager;
class IQtHelpers;
class QtFrameworkCommon;
class ISystemTrayIcon;

namespace QtFramework_Locals
{
class QtCommandEventListener;
}

class QtFramework : public Implements<IQtFramework>
{
public:
	typedef std::tuple<const unsigned char*, const unsigned char*, const unsigned char*> ResourceData;

	QtFramework(IComponentContext& contextManager);
	virtual ~QtFramework();

	void initialise(IComponentContext& contextManager);
	void finalise();

	QImage requestThumbnail(const QString& filePath, const QSize& requestedSize);

	// IQtFramework
	QQmlEngine* qmlEngine() const override;
	QFileSystemWatcher* qmlWatcher() const override;
	virtual void setIncubationTime(int msecs) override;
	virtual void incubate() override;
	QtPalette* palette() const override;
	QtGlobalSettings* qtGlobalSettings() const override;
	void addImportPath(const QString& path);

	void registerTypeConverter(IQtTypeConverter& converter) override;
	bool registerResourceData(const unsigned char* qrc_struct, const unsigned char* qrc_name,
	                          const unsigned char* qrc_data) override;
	void deregisterTypeConverter(IQtTypeConverter& converter) override;
	QVariant toQVariant(const Variant& variant, QObject* parent) const override;
	Variant toVariant(const QVariant& qVariant) const override;
	virtual void openInGraphicalShell(const char* filePath) override;
	virtual void copyTextToClipboard(const char* text) override;
	virtual void openInDefaultApp(const char* filePath) override;

	QQmlComponent* toQmlComponent(IComponent& component) override;
	QWidget* toQWidget(IView& view) override;
	void retainQWidget(IView& view) override;

	QString resolveFilePath(const char* relativePath) const override;
	QUrl resolveQmlPath(const char* relativePath) const override;

	QString resolveFilePath(const QQmlEngine& qmlEngine, const char* relativePath) const override;
	QUrl resolveQmlPath(const QQmlEngine& qmlEngine, const char* relativePath) const override;

	std::unique_ptr<ISystemTrayIcon> createSystemTrayIcon(const char* iconPath) override;

	std::unique_ptr<IAction> createAction(
		const char* id,
		std::function<void(IAction*)> func,
		std::function<bool(const IAction*)> enableFunc,
		std::function<bool(const IAction*)> checkedFunc,
		std::function<bool(const IAction*)> visibleFunc) override;

	std::unique_ptr<IAction> createAction(
		const char* id, 
		const char* text,
		const char* path,
		std::function<void(IAction*)> func,
		std::function<bool(const IAction*)> enableFunc,
		std::function<bool(const IAction*)> checkedFunc,
		std::function<bool(const IAction*)> visibleFunc, int actionOrder) override;

	std::unique_ptr<IAction> createAction(
		const char* id, 
		const char* text,
		const char* path,
		std::function<void(IAction*)> func,
		int actionOrder) override;

	std::unique_ptr<IAction> createSeperator(const char* id, const char* path, int actionOrder) override;
	std::unique_ptr<IAction> createSeperator(const char* id) override;

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
	void registerComponent(const char* id, const char* version, IComponent& component, bool supportsAsync ) override;
	void registerComponentProvider(IComponentProvider& provider) override;
	IComponent* findComponent(const TypeId& typeId, std::function<bool(const ItemRole::Id&)>& predicate,
	                          const char* version) const override;
	IComponent* findComponent(const char* componentId, const char* version) const override;

	virtual void registerDialog(const char* id, const char* version, std::shared_ptr<IDialog> dialog) override;
	virtual std::shared_ptr<IDialog> findDialog(const char* id, const char* version) override;

	virtual void registerModelExtension(const char* id, const char* version, std::function<ManagedObjectPtr()> creator) override;

	virtual void setPluginPath(const std::string& path) override;
	virtual const std::string& getPluginPath() const override;

	IPreferences* getPreferences() override;
	void doOnUIThread(std::function<void()>) override;
	void processEvents() override;

	virtual void registerQmlType(const ObjectHandle& type) override;

	virtual void showShortcutConfig() const override;

	virtual std::shared_ptr<IDialog> createDialog(const char* resource, const char* title) override;
	virtual std::shared_ptr<IDialog> createDialog(const char* resource, ObjectHandleT<DialogModel> model) override;
	virtual std::shared_ptr<IDialog> createDialog(const char* resource, ManagedObjectPtr model) override;

	virtual std::shared_ptr<IDialog> showDialog(const char* resource, 
	                                            const IDialog::Mode mode = IDialog::Mode::MODAL,
	                                            const IDialog::ClosedCallback& callback = nullptr,
	                                            const char* title = nullptr) override;

	virtual std::shared_ptr<IDialog> showDialog(ObjectHandleT<DialogModel> model,
	                                            const IDialog::Mode mode = IDialog::Mode::MODAL,
	                                            const IDialog::ClosedCallback& callback = nullptr) override;

	virtual std::shared_ptr<IDialog> showDialog(ManagedObjectPtr model,
	                                            const IDialog::Mode mode = IDialog::Mode::MODAL,
	                                            const IDialog::ClosedCallback& callback = nullptr) override;

	virtual bool shouldRedirectMessageBoxToLog() const override;
	virtual void redirectMessageBoxToLog(bool redirect) override;
	virtual IUIFramework::MessageBoxButtons displayMessageBox(const char* title, 
	                                                          const char* message, 
	                                                          unsigned int buttons, 
	                                                          MessageBoxIcon icon = MessageBoxIcon::Information,
															  const char* detail = nullptr,
															  const char* customIcon = nullptr,
															  unsigned int defaultButton = MessageBoxButtons::NoButton) override;

	virtual void loadIconData(const char* resource, ResourceType type) override;
	virtual const char* getIconUrlFromImageProvider(const char* key) const override;
	virtual void registerThumbnailProvider(std::shared_ptr<IThumbnailProvider> thumbnailProvider) override;
	virtual bool hasThumbnail(const char* filePath) const override;
	virtual void iterateDialogs(std::function<void(const IDialog& dialog)> fn) override;

	virtual Connection connectPaletteThemeChanged(PaletteThemeChangedCallback cb) override;
	virtual void setPaletteTheme(Palette::Theme theme) override;
	Palette::Theme QtFramework::getPaletteTheme() const override;
	virtual Vector4 getPaletteColor(Palette::Color color) const override;

protected:
	virtual QmlWindow* createQmlWindow();
	virtual QtWindow* createQtWindow(QIODevice& source);

private:
	virtual void makeFakeMouseRelease();
	std::shared_ptr<IDialog> createDialogInternal(std::shared_ptr<IDialog> dialog);

	std::shared_ptr<IDialog> showDialogInternal(std::shared_ptr<IDialog> dialog, 
                                                const IDialog::Mode mode,
	                                            const IDialog::ClosedCallback& callback);

	wg_future<std::unique_ptr<IView>> createViewInternal(const char* uniqueName, const char* resource,
	                                                     ResourceType type, const Variant& context,
	                                                     std::function<void(IView&)> loadedHandler, bool async);

	void createWindowInternal(const char* resource, ResourceType type, const Variant& context,
	                          std::function<void(std::unique_ptr<IWindow>&)> loadedHandler, bool async);

	void findReloadableFiles(const QString& path, const QStringList& filter, QStringList& files);
	void registerReloadableFiles();
	void registerDefaultDialogs();
	void unregisterResources();
	void onApplicationStartUp();
	void onApplicationExit();

	/*!
	Fires an event that will call function later when event is processed.
	*/
	void callLater(std::function<void(void)> function) const;

	std::unique_ptr<QtFrameworkCommon> qtFrameworkBase_;

	std::unique_ptr<QFileSystemWatcher> qmlWatcher_;

	std::vector<ResourceData> registeredResources_;

	std::vector<std::shared_ptr<IDialog>> defaultDialogs_;
	std::map<std::string, std::map<std::vector<int>, std::weak_ptr<IDialog>, ComponentVersion>> dialogs_;

	QMetaObject* constructMetaObject(QMetaObject* original);
	std::vector<std::unique_ptr<QMetaObject>> registeredTypes_;

	typedef std::tuple<std::shared_ptr<IDialog>, bool, Connection> ModelessDialogData;
	std::vector<ModelessDialogData> modelessDialogs_;
	mutable std::mutex modelessDialogMutex_;

	std::string pluginPath_;
	std::unique_ptr<QtFramework_Locals::QtCommandEventListener> commandEventListener_;
	IComponentContext& context_;
	std::unique_ptr<QObject> worker_;
	std::unique_ptr<IWindow> shortcutDialog_;
	ConnectionHolder connections_;
	InterfacePtrs interfaces_;
	bool useAsyncViewLoading_ = true;
	bool redirectMessageBoxToLog_ = false;

	struct Impl;
	std::unique_ptr<Impl> impl_;
};
} // end namespace wgt
#endif
