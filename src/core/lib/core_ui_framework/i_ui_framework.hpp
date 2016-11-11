#ifndef I_UI_FRAMEWORK_HPP
#define I_UI_FRAMEWORK_HPP

#include "i_progress_dialog.hpp"

#include "core_reflection/object_handle.hpp"
#include "core_common/wg_future.hpp"
#include "core_common/signal.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_ui_framework/i_dialog.hpp"
#include <memory>

#define DefaultProgressCancelText "Cancel"

namespace wgt
{
class IAction;
class IComponent;
class IComponentProvider;
class IView;
class IWindow;
class IPreferences;

/**
 * The UI Framework interface
 */
class IUIFramework
{
	typedef Signal<void(void)> SignalVoid;

public:
	/**
	 * Resource types
	 */
	enum class ResourceType
	{
		Buffer, ///< Buffer resource type
		File, ///< File resource type
		Url ///< Url resource type
	};

	virtual ~IUIFramework()
	{
	}

	/**
	* Options for file dialogs
	*/
	enum QtFileDialogOptions
	{
		None = 0x00,
		ShowDirsOnly = 0x01,
		DontResolveSymlinks = 0x02,
		DontConfirmOverwrite = 0x04,
		DontUseSheet = 0x08,
		DontUseNativeDialog = 0x10,
		ReadOnly = 0x20,
		HideNameFilterDetails = 0x40,
		DontUseCustomDirectoryIcons = 0x80
	};

	/**
	 * Create an action
	 *
	 * @param func  Action function
	 * @param enableFunc Enable function for this action
	 * @param checkedFunc checked function for this action
	 * @return IAction* The created action instance
	 */
	virtual std::unique_ptr<IAction> createAction(
	const char* id, std::function<void(IAction*)> func,
	std::function<bool(const IAction*)> enableFunc = [](const IAction*) { return true; },
	std::function<bool(const IAction*)> checkedFunc = std::function<bool(const IAction*)>(nullptr)) = 0;

	virtual std::unique_ptr<IAction> createAction(
	const char* id, const char* text, const char* path, std::function<void(IAction*)> func,
	std::function<bool(const IAction*)> enableFunc = [](const IAction*) { return true; },
	std::function<bool(const IAction*)> checkedFunc = std::function<bool(const IAction*)>(nullptr),
	int actionOrder = 0) = 0;

	/**
	 * Create component
	 *
	 * @param resource Resource data
	 * @param type The resource type
	 * @return IComponent The created component instance
	 */
	virtual std::unique_ptr<IComponent> createComponent(const char* resource, ResourceType type) = 0;

	/**
	 * DEPRECATED
	 */
	virtual std::unique_ptr<IView> createView(const char* resource, ResourceType type,
	                                          const Variant& context = Variant()) = 0;
	virtual std::unique_ptr<IView> createView(const char* uniqueName, const char* resource, ResourceType type,
	                                          const Variant& context = Variant()) = 0;
	virtual std::unique_ptr<IWindow> createWindow(const char* resource, ResourceType type,
	                                              const Variant& context = Variant()) = 0;

	virtual void enableAsynchronousViewCreation(bool enabled) = 0;

	virtual wg_future<std::unique_ptr<IView>> createViewAsync(const char* uniqueName, const char* resource,
	                                                          ResourceType type, const Variant& context = Variant(),
	                                                          std::function<void(IView&)> loadedHandler = [](IView&) {
		                                                      }) = 0;
	virtual void createWindowAsync(const char* resource, ResourceType type, const Variant& context = Variant(),
	                               std::function<void(std::unique_ptr<IWindow>&)> loadedHandler =
	                               [](std::unique_ptr<IWindow>&) {}) = 0;

	enum
	{
		DefaultProgressMinimum = 0,
		DefaultProgressMaximum = 100,
		DefaultProgressDuration = 4000
	};

	/*!
	    Creates a cancelable progress dialog with the specified title, label, and maximum of 100.
	*/
	IProgressDialogPtr createProgressDialog(const std::string& title, const std::string& label)
	{
		return createProgressDialog(title, label, DefaultProgressCancelText, DefaultProgressMinimum,
		                            DefaultProgressMaximum, std::chrono::milliseconds(DefaultProgressDuration));
	}

	/*!
	    Creates a cancelable progress dialog with the specified title, label, and maximum progress.
	*/
	IProgressDialogPtr createProgressDialog(const std::string& title, const std::string& label, uint32_t maximum)
	{
		return createProgressDialog(title, label, DefaultProgressCancelText, DefaultProgressMinimum, maximum,
		                            std::chrono::milliseconds(DefaultProgressDuration));
	}

	/*!
	    Creates a progress dialog with the specified title, label, and maximum progress.
	    If cancelText is empty the progress can't be canceled, otherwise it is used for the cancel button
	*/
	IProgressDialogPtr createProgressDialog(const std::string& title, const std::string& label,
	                                        const std::string& cancelText, uint32_t maximum)
	{
		return createProgressDialog(title, label, cancelText, DefaultProgressMinimum, maximum,
		                            std::chrono::milliseconds(DefaultProgressDuration));
	}

	/*!
	    Creates a progress dialog with the specified title, label, and maximum progress.
	    If cancelText is empty the progress can't be canceled, otherwise it is used for the cancel button
	*/
	IProgressDialogPtr createProgressDialog(const std::string& title, const std::string& label,
	                                        const std::string& cancelText, uint32_t minimum, uint32_t maximum)
	{
		return createProgressDialog(title, label, cancelText, minimum, maximum,
		                            std::chrono::milliseconds(DefaultProgressDuration));
	}

	/*!
	    Creates a progress dialog with the specified title, label, and maximum progress
	    The dialog will only display if duration passes.
	    If cancelText is empty the progress can't be canceled, otherwise it is used for the cancel button
	*/
	virtual IProgressDialogPtr createProgressDialog(const std::string& title, const std::string& label,
	                                                const std::string& cancelText, uint32_t minimum, uint32_t maximum,
	                                                std::chrono::milliseconds duration) = 0;

	/*!
	    Creates a cancelable modeless progress dialog with the specified title, label, and maximum of 100.
	    Each update the callback will be called so work can be done and progress made
	*/
	IProgressDialog* createModelessProgressDialog(const std::string& title, const std::string& label,
	                                              ModelessProgressCallback callback)
	{
		return createModelessProgressDialog(title, label, DefaultProgressCancelText, DefaultProgressMinimum,
		                                    DefaultProgressMaximum, std::chrono::milliseconds(DefaultProgressDuration),
		                                    callback);
	}

	/*!
	Creates a indeterminate modal progress dialog with the specified title and label.
	*/
	virtual IProgressDialogPtr createIndeterminateProgressDialog(const std::string& title,
	                                                             const std::string& label) = 0;

	/*!
	    Creates a cancelable modeless progress dialog with the specified title, label, and maximum progress.
	    Each update the callback will be called so work can be done and progress made
	*/
	IProgressDialog* createModelessProgressDialog(const std::string& title, const std::string& label, uint32_t maximum,
	                                              ModelessProgressCallback callback)
	{
		return createModelessProgressDialog(title, label, DefaultProgressCancelText, DefaultProgressMinimum, maximum,
		                                    std::chrono::milliseconds(DefaultProgressDuration), callback);
	}

	/*!
	    Creates a modeless progress dialog with the specified title, label, and maximum progress.
	    If cancelText is empty the progress can't be canceled, otherwise it is used for the cancel button
	    Each update the callback will be called so work can be done and progress made
	*/
	IProgressDialog* createModelessProgressDialog(const std::string& title, const std::string& label,
	                                              const std::string& cancelText, uint32_t maximum,
	                                              ModelessProgressCallback callback)
	{
		return createModelessProgressDialog(title, label, cancelText, DefaultProgressMinimum, maximum,
		                                    std::chrono::milliseconds(DefaultProgressDuration), callback);
	}

	/*!
	    Creates a modeless progress dialog with the specified title, label, and maximum progress.
	    If cancelText is empty the progress can't be canceled, otherwise it is used for the cancel button
	    Each update the callback will be called so work can be done and progress made
	*/
	IProgressDialog* createModelessProgressDialog(const std::string& title, const std::string& label,
	                                              const std::string& cancelText, uint32_t minimum, uint32_t maximum,
	                                              ModelessProgressCallback callback)
	{
		return createModelessProgressDialog(title, label, cancelText, minimum, maximum,
		                                    std::chrono::milliseconds(DefaultProgressDuration), callback);
	}

	/*!
	    Creates a modeless progress dialog with the specified title, label, and maximum progress
	    The dialog will only display if duration passes.
	    If cancelText is empty the progress can't be canceled, otherwise it is used for the cancel button
	    Each update the callback will be called so work can be done and progress made
	*/
	virtual IProgressDialog* createModelessProgressDialog(const std::string& title, const std::string& label,
	                                                      const std::string& cancelText, uint32_t minimum,
	                                                      uint32_t maximum, std::chrono::milliseconds duration,
	                                                      ModelessProgressCallback callback) = 0;

	/*!
	Shows an open file dialog with default options and returns list of files. Caption for the dialog, working directory
	and file
	filter are passed in. Returns an existing file path selected by user.
	*/
	virtual const std::vector<std::string> showOpenFileDialog(const std::string& caption, const std::string& directory,
	                                                          const std::string& filter,
	                                                          const QtFileDialogOptions& options) = 0;

	/*!
	Shows a save file dialog with default options. Caption for the dialog, working directory and file
	filter are passed in. Returns the file path with the new filename.
	*/
	virtual const std::string showSaveAsFileDialog(const std::string& caption, const std::string& directory,
	                                               const std::string& filter, const QtFileDialogOptions& options) = 0;

	/*!
	Shows a file directory dialog with default options. Caption for the dialog, working directory are passed in.
	Return an existing directory selected by user.
	*/
	virtual const std::string showSelectDirectoryDialog(const std::string& caption, const std::string& directory,
	                                                    const QtFileDialogOptions& options) = 0;

	virtual void loadActionData(const char* resource, ResourceType type) = 0;
	virtual void registerComponent(const char* id, const char* version, IComponent& component) = 0;
	virtual void registerComponentProvider(IComponentProvider& provider) = 0;
	virtual IComponent* findComponent(const TypeId& typeId, std::function<bool(const ItemRole::Id&)>& predicate,
	                                  const char* version) const = 0;

	virtual void registerDialog(const char* id, const char* version, std::shared_ptr<IDialog> dialog) = 0;
	virtual std::shared_ptr<IDialog> findDialog(const char* id, const char* version) = 0;

	virtual void setPluginPath(const std::string& path) = 0;
	virtual const std::string& getPluginPath() const = 0;

	virtual void showShortcutConfig() const = 0;

	virtual std::unique_ptr<IDialog> createDialog(const char* resource, ObjectHandleT<DialogModel> = nullptr) = 0;

	virtual void showDialog(const char* resource, const IDialog::Mode mode = IDialog::Mode::MODAL,
	                        const IDialog::ClosedCallback& callback = nullptr) = 0;

	virtual void showDialog(const char* resource, ObjectHandleT<DialogModel> model,
	                        const IDialog::Mode mode = IDialog::Mode::MODAL,
	                        const IDialog::ClosedCallback& callback = nullptr) = 0;

	enum MessageBoxButtons
	{
		Ok = 0x1,
		Cancel = 0x2,
		Save = 0x4,
		SaveAll = 0x8,
		Yes = 0x10,
		No = 0x20,
	};

	virtual int displayMessageBox(const char* title, const char* message, int buttons) = 0;

	virtual void registerQmlType(ObjectHandle type) = 0;
	virtual IPreferences* getPreferences() = 0;
	virtual void doOnUIThread(std::function<void()>) = 0;

	SignalVoid signalKeyBindingsChanged;
};
} // end namespace wgt
#endif // I_UI_FRAMEWORK_HPP
