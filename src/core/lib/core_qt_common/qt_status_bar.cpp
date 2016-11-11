//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  qt_status_bar.cpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "qt_status_bar.hpp"

#include <QStatusBar>

namespace wgt
{
QtStatusBar::QtStatusBar(QStatusBar& qStatusBar) : qStatusBar_(qStatusBar)
{
}

void QtStatusBar::showMessage(const char* message, int timeout /*= 0*/)
{
	qStatusBar_.showMessage(message, timeout);
}

void QtStatusBar::clearMessage()
{
	qStatusBar_.clearMessage();
}
} // end namespace wgt
