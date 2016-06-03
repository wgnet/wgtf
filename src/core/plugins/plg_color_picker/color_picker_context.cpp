#include "color_picker_context.hpp"

#include <QColor>
#include <QPoint>
#include <QApplication>
#include <QPixmap>
#include <QDesktopWidget>
#include <QScreen>

#include "core_reflection/property_accessor.hpp"

namespace wgt
{
namespace
{
	class GlobalMouseMoveEventFilter;

	GlobalMouseMoveEventFilter * s_GlobalMouseMoveEventFilter = nullptr;

	class GlobalMouseMoveEventFilter
		: public QObject
	{
	public:
		GlobalMouseMoveEventFilter( PropertyAccessor & accessor )
			: accessor_( accessor )
		{
		}

		bool eventFilter( QObject * object, QEvent * event )
		{
			QEvent::Type type = event->type();
			if (type == QEvent::WindowDeactivate||
				type == QEvent::ApplicationDeactivate) 
			{
				QApplication::instance()->removeEventFilter( s_GlobalMouseMoveEventFilter );
				delete s_GlobalMouseMoveEventFilter;
				s_GlobalMouseMoveEventFilter = nullptr;
				return true;
			}
			else if( event->type() == QEvent::MouseButtonRelease )
			{
				QApplication::instance()->removeEventFilter( s_GlobalMouseMoveEventFilter );
				delete s_GlobalMouseMoveEventFilter;
				s_GlobalMouseMoveEventFilter = nullptr;
                                return false;
			}
			else if( event->type() == QEvent::MouseMove )
			{
				const QDesktopWidget *desktop = QApplication::desktop();
				QPoint p = desktop->mapFromGlobal(QCursor::pos());
				const QPixmap pixmap = QGuiApplication::screens().at(desktop->screenNumber())->grabWindow(desktop->winId(),
																											p.x(), p.y(), 1, 1);
				QImage i = pixmap.toImage();
				QColor pixel = i.pixel(0, 0);
				auto red = pixel.red();
				auto green = pixel.green();
				auto blue = pixel.blue();
				accessor_.setValue( Vector4( red / 255.0f, green / 255.0f, blue/ 255.0f, 1.0f ) );
			}
			// standard event processing
			return QObject::eventFilter(object, event );
		};

		PropertyAccessor accessor_;
	};

	
}

void ColorPickerContext::startObservingColor()
{
	if( s_GlobalMouseMoveEventFilter == nullptr )
	{
		auto defManager = Context::queryInterface< IDefinitionManager >();
		auto def = defManager->getDefinition< ColorPickerContext >();
		auto property = def->bindProperty( "pixelColor", this );
		s_GlobalMouseMoveEventFilter = new GlobalMouseMoveEventFilter( property );
	}
	QApplication::instance()->installEventFilter( s_GlobalMouseMoveEventFilter );
}


void ColorPickerContext::endObservingColor()
{
	QApplication::instance()->removeEventFilter( s_GlobalMouseMoveEventFilter );
	delete s_GlobalMouseMoveEventFilter;
	s_GlobalMouseMoveEventFilter = nullptr;
}

Vector4 ColorPickerContext::grabScreenColor(Vector2 p )
{
	const QDesktopWidget *desktop = QApplication::desktop();
	const QPixmap pixmap = QGuiApplication::screens().at(desktop->screenNumber())->grabWindow(desktop->winId(),
																								p.x, p.y, 1, 1);
	QImage i = pixmap.toImage();
	QColor pixel = i.pixel(0, 0);
	auto red = pixel.red();
	auto green = pixel.green();
	auto blue = pixel.blue();
	return Vector4( red / 255.0f, green / 255.0f, blue/ 255.0f, 1.0f );
}
} // end namespace wgt
