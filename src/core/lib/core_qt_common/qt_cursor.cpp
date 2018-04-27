#include "qt_cursor.hpp"

#include "core_common/assert.hpp"
#include <QPixmap>

namespace wgt
{
QtCursor::QtCursor(std::uint8_t id)
	: qCursor_(static_cast<Qt::CursorShape>(id))
{
}

QtCursor::QtCursor(const char* filename, int hotX, int hotY)
	: qCursor_(QPixmap(filename), hotX, hotY)
{
	// If the cursor failed to load make sure the path is correct and that the resource exists in you .qrc file
	// You may need to regenerate your solution when adding new cursors to your plugin
	TF_ASSERT(qCursor_.shape() == Qt::BitmapCursor);
}

CursorId QtCursor::id() const
{
	auto shape = qCursor_.shape();
	return CursorId::make(shape, nativeCursor());
}

void* QtCursor::nativeCursor() const
{
	return (void*)&qCursor_;
}
} // end namespace wgt
