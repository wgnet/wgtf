#include "qt_tool_bar.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_logging/logging.hpp"
#include "core_string_utils/string_utils.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include <QToolBar>
#include <assert.h>

namespace wgt
{
QtToolBar::QtToolBar( QToolBar & qToolBar, const char * windowId )
	: QtMenu( qToolBar, windowId )
	, qToolBar_( qToolBar )
{
	qToolBar_.setVisible( false );
}

void QtToolBar::addAction( IAction & action, const char * path )
{
	auto qAction = getQAction(action);
	if(qAction == nullptr)
	{
		qAction = createQAction( action );
	}
	assert(qAction != nullptr);

	// TODO: deal with nested tool bars
	qToolBar_.addAction( qAction );

	qToolBar_.setVisible( true );
}

void QtToolBar::removeAction( IAction & action )
{
	auto qAction = getQAction( action );
	if (qAction == nullptr)
	{
		NGT_ERROR_MSG("Target action '%s' '%s' does not exist\n", action.text(), StringUtils::join(action.paths(), ';').c_str());
		return;
	}

	qToolBar_.removeAction( qAction );
	
	destroyQAction( action );
}
} // end namespace wgt
