#pragma once

#include "qt_view_common.hpp"
#include "qt_connection_holder.hpp"
#include "core_qt_common/qt_new_handler.hpp"
#include "core_ui_framework/i_cursor.hpp"

#include <memory>
#include <vector>
#include <QObject>
#include <QWidget>

class QIODevice;

namespace wgt
{
class IMenu;

class QtView : public QObject, public QtViewCommon
{
	Q_OBJECT

public:
	QtView(const char* id, QIODevice& source);
	virtual ~QtView();

	virtual void update() override;
	virtual void reload() override;
	virtual std::vector<IMenu*> menus() const override;
	virtual void setFocus(bool focus) override;

	virtual CursorId getCursor() const override;
	virtual void setCursor(CursorId cursorId) override;
	virtual void unsetCursor() override;

	void setLayoutHint(const QStringList& value);
	void setWindowId(const char* windowId);

private:
	void initialise();

	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
}