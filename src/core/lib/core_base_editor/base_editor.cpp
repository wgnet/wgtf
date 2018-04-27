#include "base_editor.hpp"

#include "core_reflection/object_handle.hpp"
#include "asset_manager/i_asset_manager.hpp"

#include "core_data_model/abstract_item_model.hpp"

#include "core_string_utils/file_path.hpp"
#include "core_string_utils/string_utils.hpp"

#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"

#include "core_common/assert.hpp"

#include "editor/i_document.hpp"
#include "editor/i_editor.hpp"
#include "editor/i_editor_common.hpp"

#include "interfaces/application_settings/i_application_settings.hpp"
#include "interfaces/version_control/i_file_status_manager.hpp"

#include <unordered_map>
#include <set>

namespace wgt
{
// Class serves as a holder for depends and data, actual implementation is in class itself.
class BaseEditor::Implementation: public Depends<IEditor, IUIFramework, IUIApplication, IApplicationSettings, IEditorCommon, IAssetManager, IFileStatusManager> {
public:
	Implementation(const char* editorName)
		: editorName_(editorName == nullptr ? "" : editorName)
		, defaultResourceName_("New")
	{
	}

public:
	std::string defaultResourceName_;
	std::string defaultResourceType_;
	std::string lastSaveFolder_;
	std::vector<std::unique_ptr<IAction>> actions_;
	std::vector<std::unique_ptr<IAction>> editorOnlyActions_;
	std::string editorName_;
	ConnectionHolder connections_;
};

BaseEditor::BaseEditor(const char* editorName) : pImpl_(std::make_unique<BaseEditor::Implementation>(editorName))
{
}

BaseEditor::~BaseEditor()
{
	finalize();
}

void BaseEditor::initialize()
{
	createDefaultFileActions();
	connectToLoadRequest();
}

void BaseEditor::finalize()
{
	removeAllActions();
	pImpl_->actions_.clear();
	pImpl_->editorOnlyActions_.clear();
	pImpl_->connections_.clear();
}

void BaseEditor::createDefaultFileActions(int actionTypes)
{
	std::string actionId;
	if (actionTypes & eNew)
	{
		actionId = pImpl_->editorName_ + "New";
		createAction(actionId.c_str(), &BaseEditor::actionNew);
	}
	if (actionTypes & eNewFromTemplate)
	{
		actionId = pImpl_->editorName_ + "NewFromTemplate";
		createAction(actionId.c_str(), &BaseEditor::actionNewFromTemplate);
	}
	if (actionTypes & eNewFromFile)
	{
		actionId = pImpl_->editorName_ + "NewFromFile";
		createAction(actionId.c_str(), &BaseEditor::actionNewFromFile);
	}
	if (actionTypes & eOpen)
	{
		actionId = pImpl_->editorName_ + "Open";
		createAction(actionId.c_str(), &BaseEditor::actionOpen);
	}
	if (actionTypes & eClose)
	{
		actionId = pImpl_->editorName_ + "Close";
		createAction(actionId.c_str(), &BaseEditor::actionClose, true);
	}
	if (actionTypes & eSave)
	{
		actionId = pImpl_->editorName_ + "Save";
		createAction(actionId.c_str(), &BaseEditor::actionSave, true);
	}
	if (actionTypes & eSaveAs)
	{
		actionId = pImpl_->editorName_ + "SaveAs";
		createAction(actionId.c_str(), &BaseEditor::actionSaveAs, true);
	}
}

void BaseEditor::connectToLoadRequest()
{
	auto editorCommon = pImpl_->get<IEditorCommon>();
	TF_ASSERT(editorCommon != nullptr);
	if (editorCommon)
	{
		pImpl_->connections_.add(editorCommon->connectLoadAsset([this](const char* assetPath, bool activateLoadingFile, const Variant& loadedResources)
		{
			if (isValidAssetPath(assetPath))
			{
				processAssetLoadRequest(assetPath, activateLoadingFile, loadedResources);
			}
		}));
		pImpl_->connections_.add(editorCommon->connectGetAssetLoadedResources([this](const char* assetPath, Variant& loadedResources)
		{
			if (isValidAssetPath(assetPath))
			{
				getLoadedResources(assetPath, loadedResources);
			}
		}));
	}
}

void BaseEditor::defaultResourceName(const char* name)
{
	pImpl_->defaultResourceName_ = name;
}

void BaseEditor::defaultResourceType(const char* type)
{
	pImpl_->defaultResourceType_ = type;
}

void BaseEditor::bindDocument(IDocument* doc)
{
	if (!doc)
	{
		return;
	}

	auto updateObjectHandleCheckoutCallback = [this](const ObjectHandle& handle) {updateCheckoutState(handle);};
	pImpl_->connections_.add(doc->connectUpdateEditState(updateObjectHandleCheckoutCallback));
	auto updateFileCheckoutCallback = [this](const char* filePath) {updateCheckoutState(filePath); };
	pImpl_->connections_.add(doc->connectUpdateEditState(updateFileCheckoutCallback));
	pImpl_->connections_.add(doc->connectSave([this](const IDocument*) {actionSave(); }));
	pImpl_->connections_.add(doc->connectAssetDropped([this](const std::vector<std::string>& assetPaths) { onAssetsDropped(assetPaths); }));
	pImpl_->connections_.add(doc->connectClose([this]() {
		clearCheckoutState();
		onClose();
	}));
}

void BaseEditor::actionNew()
{
	static int postfix = 1;
	const auto name(pImpl_->defaultResourceName_ + std::to_string(postfix++));
	IDocument* doc = nullptr;
	auto editor = pImpl_->get<IEditor>();
	TF_ASSERT(editor != nullptr);
	if (editor)
	{
		doc = editor->create(name.c_str(), pImpl_->defaultResourceType_.c_str());
		bindDocument(doc);
	}

	onNew(name, doc);
}

void BaseEditor::actionNewFromTemplate()
{
	onNewFromTemplate();
}

void BaseEditor::actionNewFromFile()
{
	auto uiFramework = pImpl_->get<IUIFramework>();
	TF_ASSERT(uiFramework != nullptr);
	if (!uiFramework)
	{
		return;
	}
	IDocument* doc = nullptr;
	auto path = uiFramework->showOpenFileDialog("Open", lastSaveFolder(), fileOpenFilter(), IUIFramework::None);
	if (!path.empty())
	{
		auto editor = pImpl_->get<IEditor>();
		TF_ASSERT(editor != nullptr);
		if (editor)
		{
			doc = editor->open(path.front().c_str());

			if (doc == nullptr)
			{
				return;
			}

			bindDocument(doc);

			if (path.front() == doc->getFilePath())
			{
				clearCheckoutState();
			}
		}

		onNewFromFile(path.front().c_str(), doc);
		auto editorCommon = pImpl_->get<IEditorCommon>();
		TF_ASSERT(editorCommon != nullptr);
		if (editorCommon)
		{
			editorCommon->addToRecentFiles(path.front().c_str());
		}
	}
}

void BaseEditor::actionOpen()
{
	auto uiFramework = pImpl_->get<IUIFramework>();
	TF_ASSERT(uiFramework != nullptr);
	if (!uiFramework)
	{
		return;
	}
	IDocument* doc = nullptr;
	auto path = uiFramework->showOpenFileDialog("Open", lastSaveFolder(), fileOpenFilter(), IUIFramework::None);
	if (!path.empty())
	{
		auto editor = pImpl_->get<IEditor>();
		TF_ASSERT(editor != nullptr);
		if (editor)
		{
			doc = editor->open(path.front().c_str());

			if (doc == nullptr)
			{
				return;
			}

			bindDocument(doc);

			if (path.front() == doc->getFilePath())
			{
				clearCheckoutState();
			}
		}

		onOpen(path.front().c_str(), doc);
		auto editorCommon = pImpl_->get<IEditorCommon>();
		TF_ASSERT(editorCommon != nullptr);
		if (editorCommon)
		{
			editorCommon->addToRecentFiles(path.front().c_str());
		}
	}
}

void BaseEditor::actionClose()
{
	auto editor = pImpl_->get<IEditor>();
	TF_ASSERT(editor != nullptr);
	if (editor)
	{
		editor->close();
	}
}

void BaseEditor::actionSave()
{
	if (!hasFileName())
	{
		return actionSaveAs();
	}

	if(onSave())
	{
		auto editor = pImpl_->get<IEditor>();
		TF_ASSERT(editor != nullptr);
		if (editor)
		{
			editor->save();
		};
	}
}

void BaseEditor::actionSaveAs()
{
	auto uiFramework = pImpl_->get<IUIFramework>();
	TF_ASSERT(uiFramework != nullptr);
	if (!uiFramework)
	{
		return;
	}

	auto path = uiFramework->showSaveAsFileDialog("Save As", lastSaveFolder(), fileSaveFilter(), IUIFramework::None);
	if (!path.empty())
	{
		clearCheckoutState();
		setLastSaveFolder(FilePath::getFolder(path));
		if(onSaveAs(path.c_str()))
		{
			auto editor = pImpl_->get<IEditor>();
			TF_ASSERT(editor != nullptr);
			if (editor)
			{
				editor->saveAs(path.c_str());
			}
			auto assetManager = pImpl_->get<IAssetManager>();
			if (assetManager)
			{
				auto assetModel = assetManager->assetModel();
				if (assetModel)
				{
					assetModel->revert();
				}
			}
		}
	}
}

std::string BaseEditor::lastSaveFolder() const
{
	return pImpl_->lastSaveFolder_.c_str();
}

void BaseEditor::setLastSaveFolder(const std::string& path)
{
	pImpl_->lastSaveFolder_ = path;
}

void BaseEditor::setDirty(bool dirty) {
	auto editor = pImpl_->get<IEditor>();
	TF_ASSERT(editor != nullptr);
	if (editor)
	{
		editor->setDirty(dirty);
	}
}

void BaseEditor::addAction(std::unique_ptr<IAction>&& action, bool editorOnly)
{
	auto uiApplication = pImpl_->get<IUIApplication>();
	if (uiApplication == nullptr)
	{
		return;
	}
	if (editorOnly)
	{
		pImpl_->editorOnlyActions_.push_back(std::move(action));
		uiApplication->addAction(*pImpl_->editorOnlyActions_.back());
	}
	else
	{
		pImpl_->actions_.push_back(std::move(action));
		uiApplication->addAction(*pImpl_->actions_.back());
	}
}

void BaseEditor::addEditorOnlyActions()
{
	auto uiApplication = pImpl_->get<IUIApplication>();
	if (uiApplication == nullptr)
	{
		return;
	}

	for (auto& action : pImpl_->editorOnlyActions_)
	{
		// Avoid having action added twice.
		uiApplication->removeAction(*action);
		uiApplication->addAction(*action);
	}
}

void BaseEditor::removeEditorOnlyActions()
{
	auto uiApplication = pImpl_->get<IUIApplication>();
	if (uiApplication == nullptr)
	{
		return;
	}

	for (auto& action : pImpl_->editorOnlyActions_)
	{
		uiApplication->removeAction(*action);
	}
}

void BaseEditor::removeAllActions()
{
	auto uiApplication = pImpl_->get<IUIApplication>();
	if (uiApplication == nullptr)
	{
		return;
	}

	for (auto& action : pImpl_->actions_)
	{
		uiApplication->removeAction(*action);
	}
	removeEditorOnlyActions();
}

bool BaseEditor::isValidAssetPath(const char* assetPath) const
{
	const std::string path = assetPath;
	if (!path.empty())
	{
		std::string ext = FilePath::getExtension(path);
		const auto allEextensions = extensions();
		return std::find(allEextensions.begin(), allEextensions.end(), ext) != allEextensions.end();
	}
	return false;
}

void BaseEditor::processAssetLoadRequest(const char* assetPath, bool activateLoadingFile, const Variant& resourcesToLoad)
{
	auto editor = pImpl_->get<IEditor>();
	TF_ASSERT(editor != nullptr);
	if (!editor)
	{
		return;
	}

	auto doc = editor->open(assetPath, activateLoadingFile);

	if (doc == nullptr)
	{
		// A new document was not opened.
		return;
	}

	if (std::string(assetPath) == doc->getFilePath())
	{
		clearCheckoutState();
	}

	if (auto editorCommon = pImpl_->get<IEditorCommon>())
	{
		editorCommon->setSelectedSceneNodeIds(std::vector<uintptr_t>());
	}

	bindDocument(doc);
	onOpen(assetPath, doc);
	loadResources(assetPath, resourcesToLoad);
}

IUIFramework* BaseEditor::getUIFramework() const
{
	return pImpl_->get<IUIFramework>();
}

const char* BaseEditor::getResourceFileName(const ObjectHandle& handle) const
{
	return nullptr;
}

void BaseEditor::updateCheckoutState(const char* path)
{
	IFileStatusManager* fileStatusManager = pImpl_->get<IFileStatusManager>();

	if (fileStatusManager == nullptr || !path || fileStatusManager->getFileState(path) != FileEditState::Clean)
	{
		return;
	}

	if (auto editorCommon = pImpl_->get<IEditorCommon>())
	{
		editorCommon->releaseCurrentViewportFocus();
	}

	if (fileStatusManager->requestEdit(path) != FileEditState::Dirty)
	{
		return;
	}

	setDirty(true);
}

void BaseEditor::updateCheckoutState(const ObjectHandle& handle)
{
	if (!handle.isValid())
	{
		return;
	}

	ObjectHandle root = handle;

	while (root.parent().isValid())
	{
		root = root.parent();
	}

	auto path = getResourceFileName(root);

	if (path == nullptr || *path == char(0))
	{
		return;
	}

	updateCheckoutState(path);
}

bool BaseEditor::shouldClearCheckoutState(IFileStatusManager& fileStatusManager, const char* path) const
{
	if (!path || *path == 0)
	{
		return false;
	}

	return fileStatusManager.getFileState(path) == FileEditState::Scratched;
}

void BaseEditor::clearCheckoutState(IFileStatusManager& fileStatusManager, const char* path) const
{
	if (!shouldClearCheckoutState(fileStatusManager, path))
	{
		return;
	}

	fileStatusManager.removeFileStatus(path);
}

void BaseEditor::iterateCheckoutStateFiles(const std::function<void(const char* path)>& operation)
{
}

void BaseEditor::clearCheckoutState()
{
	IFileStatusManager* fileStatusManager = pImpl_->get<IFileStatusManager>();

	if (fileStatusManager == nullptr)
	{
		return;
	}

	auto operation = [this, fileStatusManager](const char* path)
	{
		clearCheckoutState(*fileStatusManager, path);
	};

	iterateCheckoutStateFiles(operation);
}
}
