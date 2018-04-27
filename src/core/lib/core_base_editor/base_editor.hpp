#pragma once

#include "core_dependency_system/depends.hpp"
#include "core_common/signal.hpp"

#include <memory>
#include <vector>
#include <string>

namespace wgt
{

class IAction;
class IEditor;
class IUIFramework;
class IComponentContext;
class IDocument;
class ObjectHandle;
class IFileStatusManager;
class Variant;

/**
* Base class for editors that provides default implementation for common actions.
*/
class BaseEditor {
protected:
	enum defaultActions
	{
		eNone = 0x0000000,
		eNew =	0x0000001,
		eNewFromTemplate = 0x0000010,
		eNewFromFile = 0x0000100,
		eOpen = 0x0001000,
		eClose = 0x0010000,
		eSave =	0x0100000,
		eSaveAs = 0x1000000,
		eAll = 0x1111111
	};

	BaseEditor(const char* editorName = nullptr);
	virtual ~BaseEditor();

	template <typename TReturn, typename TObject>
	void createAction(const char* name, TReturn(TObject::*pmfn)(), bool editorOnly = false)
	{
		TObject* derivedPtr = static_cast<TObject*>(this);
		addAction(getUIFramework()->createAction(name, [pmfn, derivedPtr](IAction*) { (derivedPtr->*pmfn)(); }), editorOnly);
	}

	template<typename TFunc>
	void createAction(const char* name, TFunc fn, typename std::enable_if<!std::is_member_function_pointer<TFunc>::value>::type* = nullptr, bool editorOnly = false)
	{
		addAction(getUIFramework()->createAction(name, fn), editorOnly);
	}

	template<typename TFunc>
	void createAction(const char* name, const char* text, const char* path, TFunc fn, bool editorOnly = false)
	{
		addAction(getUIFramework()->createAction(name, text, path, fn), editorOnly);
	}

	template<typename TFunc>
	void createAction(const char* name, const char* text, const char* path, TFunc fn, typename std::enable_if<!std::is_member_function_pointer<TFunc>::value>::type* = nullptr, bool editorOnly = false)
	{
		addAction(getUIFramework()->createAction(name, text, path, fn), editorOnly);
	}

	template<typename TReturn, typename TObject, typename TPredicateFunc>
	void createAction(const char* name, TReturn(TObject::*pmfn)(), TPredicateFunc pmfnPredicate, bool editorOnly = false)
	{
		static_assert(std::is_member_function_pointer<decltype(pmfn)>::value, "Must be pointer to member function");
		static_assert(std::is_member_function_pointer<decltype(pmfnPredicate)>::value, "Must be pointer to member function");
		TObject* derivedPtr = static_cast<TObject*>(this);
		addAction(getUIFramework()->createAction(name,
			[pmfn, derivedPtr](IAction*) { (derivedPtr->*pmfn)(); },
			[pmfnPredicate, derivedPtr](const IAction*) { return (derivedPtr->*pmfnPredicate)(); }), editorOnly);
	}

	template<typename TFunc, typename TPredicateFunc>
	void createAction(const char* name, TFunc fn, TPredicateFunc fnPredicate, bool editorOnly = false)
	{
		addAction(getUIFramework()->createAction(name,
			[fn](IAction*) { fn(); },
			[fnPredicate](const IAction*) { return fnPredicate(); }), editorOnly);
	}

	template<typename TObject>
	void createComplexAction(const char* name, void(TObject::*fn)(IAction*), bool(TObject::*fnEnabled)(const IAction*), bool(TObject::*fnChecked)(const IAction*), bool(TObject::*fnVisible)(const IAction*), bool editorOnly = false)
	{
		TObject* derivedPtr = static_cast<TObject*>(this);
		addAction(getUIFramework()->createAction(name,
			[fn, derivedPtr](IAction* action) { (derivedPtr->*fn)(action); },
			[fnEnabled, derivedPtr](const IAction* action) { return fnEnabled ? (derivedPtr->*fnEnabled)(action) : true; },
			[fnChecked, derivedPtr](const IAction* action) { return fnChecked ? (derivedPtr->*fnChecked)(action) : false; },
			[fnVisible, derivedPtr](const IAction* action) { return fnVisible ? (derivedPtr->*fnVisible)(action) : true; }), editorOnly);
	}

	/**
	* Calls to  \createDefaultFileActions \connectToLoadRequest \updateRecentFileActions and addActions()
	*/
	virtual void initialize();

	/**
	* De-registers and destroys all actions.
	*/
	virtual void finalize();

	/**
	* Creates new, open, close, save, saveAs actions.
	*/
	void createDefaultFileActions(int actionTypes = eAll);


	/**
	* Connects to load request signal from IEditorCommon.
	*/
	void connectToLoadRequest();

	/**
	* Provides list of handled extension.
	* Override to customize.
	* @return vector of strings with supported extensions.
	*/
	virtual std::vector<std::string> extensions() const { return {""}; }

	/**
	* Provides file filter string for file open dialog.
	* Override to customize.
	* @return C string with custom file filter.
	*/
	virtual const char* fileOpenFilter() const { return ""; }

	/**
	* Provides file filter string for file open dialog.
	* Override to customize.
	* @return C string with custom file filter.
	*/
	virtual const char* fileSaveFilter() const { return ""; }

	/**
	* Last saved folder.
	* Override to customize.
	* @return String with last saved folder.
	*/
	virtual std::string lastSaveFolder() const;

	/**
	* Stores last saved folder.
	* Override to customize.
	* @param path String with new last saved folder.
	*/
	virtual void setLastSaveFolder(const std::string& path);
	
	/**
	* Checks is current document has a file on disk, if false action save will trigger saveAs.
	* Override to customize.
	* @return path String with new last saved folder.
	*/
	virtual bool hasFileName() const { return false; }

	/**
	* Called on action new.
	* Override to handle event.
	*/
	virtual void onNew(const std::string& name, IDocument* associatedDoc = nullptr) = 0;

	/**
	* Called on action new when using an empty template.
	* Override to handle event.
	*/
	virtual void onNewFromTemplate() {};

	/**
	* Called on action new when using a specific dsstreaming file as a template.
	* Override to handle event.
	*/
	virtual void onNewFromFile(const std::string&, IDocument* associatedDoc) {};

	/**
	* Called on action close.
	* Override to handle event.
	*/
	virtual void onClose() = 0;

	/**
	* Called on action open.
	* Override to handle event.
	* @param path path to selected file, files would have extensions listed in \extensions call or in \fileFilter.
	*/
	virtual void onOpen(const std::string& path, IDocument* associatedDoc = nullptr) = 0;

	/**
	* Called on action save.
	* Override to handle event.
	*/
	virtual bool onSave() = 0;

	/**
	* Called on action saveAs.
	* Override to handle event.
	* @param path path to selected file.
	*/
	virtual bool onSaveAs(const std::string& path) = 0;

	/** Loads resources specific for the editor */
	virtual void loadResources(const char*, const Variant&) {}
	/** Gets the current loaded resources specific for the editor */
	virtual void getLoadedResources(const char*, Variant&) const {}

	virtual void onAssetsDropped(const std::vector<std::string>&) {}

	/**
	* Call to set dirty status of current environment.
	* @param dirty dirty status.
	*/
	void setDirty(bool dirty);

	void actionNew();
	void actionNewFromTemplate();
	void actionNewFromFile();
	void actionOpen();
	void actionClose();
	void actionSave();
	void actionSaveAs();
	
	/**
	* Sets what a new resource created will default be called
	*/
	void defaultResourceName(const char* name);

	/**
	* Sets the type of a default resource created
	*/
	void defaultResourceType(const char* type);

	/**
	* Adds action to the action list.
	*/
	void addAction(std::unique_ptr<IAction>&& action, bool editorOnly = false);

	/**
	* Adds all editor only actions back to IUIApplication.
	*/
	void addEditorOnlyActions();

	/**
	* Removes all editor actions from IUIApplication.
	*/
	void removeEditorOnlyActions();

	virtual const char* getResourceFileName(const ObjectHandle& handle) const;
	virtual void updateCheckoutState(const char* path);

	virtual bool shouldClearCheckoutState(IFileStatusManager& fileStatusManager, const char* path) const;
	void clearCheckoutState(IFileStatusManager& fileStatusManager, const char* path) const;
	virtual void iterateCheckoutStateFiles(const std::function<void(const char* path)>& operation);
	virtual void clearCheckoutState();
	void bindDocument(IDocument* doc);

private:
	void removeAllActions();
	IUIFramework* getUIFramework() const;
	void updateCheckoutState(const ObjectHandle& handle);
	void processAssetLoadRequest(const char* assetPath, bool activateLoadingFile, const Variant& resourcesToLoad);
	bool isValidAssetPath(const char* assetPath) const;

	class Implementation;
	std::unique_ptr<Implementation> pImpl_;
};

}