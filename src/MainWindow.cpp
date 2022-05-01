/*
 * Copyright 2022, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "MainWindow.h"

#include <LayoutBuilder.h>
#include <TabView.h>

#include "LyricsView.h"


MainWindow::MainWindow()
	:
	BWindow(BRect(BPoint(0,0),BSize(500.0, 500.0)), "MediaMonitor",
		B_TITLED_WINDOW, B_AUTO_UPDATE_SIZE_LIMITS | B_QUIT_ON_WINDOW_CLOSE)
{
	SetPulseRate(0);
	CenterOnScreen();
	_InitInterface();
}


void
MainWindow::_InitInterface()
{
	BTabView* tabView = new BTabView("appletTabView");
	tabView->AddTab(new LyricsView(BRect(BPoint(0,0), BSize(300.0, 200.0))));

	BLayoutBuilder::Group<>(this, B_VERTICAL, 0.0f)
		.Add(tabView)
	.End();
}
