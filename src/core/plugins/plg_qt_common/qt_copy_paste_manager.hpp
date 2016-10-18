#ifndef QT_COPY_PASTE_MANAGER_HPP
#define QT_COPY_PASTE_MANAGER_HPP

#include "core_dependency_system/i_interface.hpp"
#include "core_copy_paste/i_copy_paste_manager.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include <vector>
#include <QObject>

class QClipboard;

namespace wgt
{
class ISerializer;
class ICommandManager;

/**
 * QtCopyPasteManager
 * Responsible for serializing copy/paste data to the system clipboard.
 */
class QtCopyPasteManager
: public Implements<ICopyPasteManager>,
  public QObject
{
public:
	QtCopyPasteManager();
	~QtCopyPasteManager();

	void init( IDefinitionManager* definitionManager, ICommandManager* commandManager );
	void fini();

	void onSelect( ICopyableObject* pObject, bool append = false ) override;
	void onDeselect( ICopyableObject* pObject, bool reset = true ) override;
	bool copy() override;
	bool paste() override;
	bool canCopy() const override;
	bool canPaste() const override;

	MimeData getClipboardContents() override;
	void setClipboardContents(MimeData& mimeData) override;

	std::string getText() override;
	void setText(std::string str) override;

	virtual Connection connectClipboardDataChanged(ClipboardCallback callback) override;

private:
	void clipboardChangedSignal();

	QClipboard * clipboard_;
	std::vector< ICopyableObject* > curObjects_;
	IDefinitionManager * definitionManager_;
	ICommandManager * commandManager_;
	Signal<ClipboardSignature> onClipboardDataChanged;
};
} // end namespace wgt
#endif // QT_COPY_PASTE_MANAGER_HPP
