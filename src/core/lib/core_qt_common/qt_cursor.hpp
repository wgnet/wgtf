#ifndef QT_CURSOR_HPP
#define QT_CURSOR_HPP

#include "core_ui_framework/i_cursor.hpp"

#include <QCursor>

namespace wgt
{
class QtCursor : public ICursor
{
public:
	QtCursor(std::uint8_t id);
	QtCursor(const char* filename, int hotX, int hotY);
	CursorId id() const override;
	void* nativeCursor() const override;

private:
	QCursor qCursor_;
};
} // end namespace wgt
#endif // QT_CURSOR_HPP
