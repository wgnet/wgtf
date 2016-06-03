#include "qt_application.hpp"

#include "core_automation/interfaces/automation_interface.hpp"
#include "core_common/ngt_windows.hpp"
#include "core_common/platform_env.hpp"

#include "core_qt_common/i_qt_framework.hpp"
#include "core_qt_common/qml_view.hpp"
#include "core_qt_common/qt_palette.hpp"
#include "core_qt_common/qt_window.hpp"

#include "core_ui_framework/i_action.hpp"

#include <cassert>

#include <QAbstractEventDispatcher>
#include <QApplication>
#include <QFont>
#include <QPalette>
#include <QStyleFactory>
#include <QTimer>
#include <QSplashScreen>

namespace wgt
{
namespace
{
	class IdleLoop : public QObject
	{
	public:
		IdleLoop( QtApplication & qtApplication, QObject * parent )
			: QObject( parent )
			, qtApplication_( qtApplication )
		{
			timer_ = new QTimer( this );
			timer_->setInterval( 10 );
			QObject::connect( timer_, &QTimer::timeout, [&]() { 
				qtApplication_.update(); 
			} );
		}

	public slots:
		void start()
		{
			qtApplication_.update();
			timer_->start();
		}

		void stop()
		{
			timer_->stop();
		}

	private:
		QtApplication & qtApplication_;
		QTimer * timer_;
	};
}

QtApplication::QtApplication( int argc, char** argv )
	: application_( nullptr )
	, argc_( argc )
	, argv_( argv )
	, qtFramework_(nullptr)
	, splash_( nullptr )
	, bQuit_( false )

{
	char ngtHome[MAX_PATH];

	if (Environment::getValue< MAX_PATH >( "NGT_HOME", ngtHome ))
	{
		QCoreApplication::addLibraryPath( ngtHome );
	}
	
	application_.reset( new QApplication( argc_, argv_ ) );

	QCoreApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
	QApplication::setDesktopSettingsAware( false );
	QApplication::setStyle( QStyleFactory::create( "Fusion" ) );
	QApplication::setFont( QFont( "Noto Sans", 9 ) );
	
	auto dispatcher = QAbstractEventDispatcher::instance();
	auto idleLoop = new IdleLoop( *this, application_.get() );
	
	QObject::connect( dispatcher, &QAbstractEventDispatcher::aboutToBlock, idleLoop, &IdleLoop::start );
	QObject::connect( dispatcher, &QAbstractEventDispatcher::awake, idleLoop, &IdleLoop::stop );

	//Splash
	QPixmap pixmap( ":/qt_app/splash" );
	splash_.reset( new QSplashScreen( pixmap ) );
	splash_->show();
}

QtApplication::~QtApplication()
{
}

void QtApplication::initialise( IQtFramework * qtFramework )
{
	qtFramework_ = qtFramework;
	assert( qtFramework_ != nullptr );
	if(qtFramework_ != nullptr)
	{
		auto palette = qtFramework_->palette();
		if (palette != nullptr)
		{
			application_->setPalette( palette->toQPalette() );
		}
	}
}

void QtApplication::finalise()
{
	signalUpdate.clear();
}

void QtApplication::update()
{
	AutomationInterface* pAutomation =
		Context::queryInterface< AutomationInterface >();
	if (pAutomation)
	{
		if (pAutomation->timedOut())
		{
			this->quitApplication();
		}
	}

	layoutManager_.update();

	signalUpdate();
}

int QtApplication::startApplication()
{
	assert( application_ != nullptr );
	signalStartUp();
	splash_->close();
	splash_ = nullptr;
	if(bQuit_)
	{
		return 0;
	}
	return application_->exec();
}

void QtApplication::quitApplication()
{
	QApplication::quit();
	bQuit_ = true;
}

void QtApplication::addWindow( IWindow & window )
{
	assert( window.getApplication() == nullptr );

	window.setApplication( this );
	layoutManager_.addWindow( window );
}

void QtApplication::removeWindow( IWindow & window )
{
	assert( window.getApplication() == this );

	layoutManager_.removeWindow( window );
	window.setApplication( nullptr );
}

void QtApplication::addView( IView & view )
{
	layoutManager_.addView( view );
}

void QtApplication::removeView( IView & view )
{
	layoutManager_.removeView( view );
}

void QtApplication::addMenu( IMenu & menu )
{
	layoutManager_.addMenu( menu );
}

void QtApplication::removeMenu( IMenu & menu )
{
	layoutManager_.removeMenu( menu );
}

void QtApplication::addAction( IAction & action )
{
	layoutManager_.addAction( action );
}

void QtApplication::removeAction( IAction & action )
{
	layoutManager_.removeAction( action );
}

void QtApplication::setWindowIcon(const char* path, const char* windowId)
{
	layoutManager_.setWindowIcon(path, windowId);
}

const Windows & QtApplication::windows() const
{
	return layoutManager_.windows();
}

/*
void QtApplication::getCommandLine()
{
	char* winCommandLine = GetCommandLine();
	std::vector<char*> parameters;
	char* position = winCommandLine;
	char* start = position;
	bool quote = false;
	bool space = false;
	size_t index = 0;

	auto copyData = [&position, &start]()->char*
	{
		size_t len = position - start;
		char* destination = new char[len + 1];
		memcpy(destination, start, len);
		destination[len] = 0;
		return destination;
	};

	while (*position)
	{
		switch (*position)
		{
		case ' ':
		case '\t':
		case '\r':
		case '\n':
			if (!quote)
			{
				parameters.push_back(copyData());
				++index;
				space = true;
			}

			break;

		case '"':
			if (!quote)
			{
				start = position;
			}

			space = false;
			quote = !quote;
			break;

		default:
			if (space)
			{
				start = position;
			}

			space = false;
		}

		++position;
	}

	if (position > winCommandLine && !whiteSpace(*(position - 1)))
	{
		parameters.push_back(copyData());
		++index;
	}

	argc = index;
	argv = new char*[argc + 1];
	argv[argc] = nullptr;

	for (unsigned int i = 0; i < index; ++i)
	{
		argv[i] = parameters[i];
	}
}

bool QtApplication::whiteSpace(char c)
{
	return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}
*/
} // end namespace wgt
