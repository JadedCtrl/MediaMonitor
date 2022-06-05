/*
 * Copyright 2022, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "MainWindow.h"

#include <LayoutBuilder.h>
#include <SplitView.h>
#include <TabView.h>

#include "CoverView.h"
#include "LyricsView.h"
#include "VolumeView.h"


MainWindow::MainWindow()
	:
	BWindow(BRect(BPoint(0,0),BSize(500.0, 500.0)), APP_NAME,
		B_TITLED_WINDOW, B_AUTO_UPDATE_SIZE_LIMITS | B_QUIT_ON_WINDOW_CLOSE)
{
	SetPulseRate(0);
	CenterOnScreen();
	_InitInterface();
}


void
MainWindow::_InitInterface()
{

	BView* controlsView = new BView("Controls", B_WILL_DRAW);
	BLayoutBuilder::Group<>(controlsView, B_VERTICAL)
		.Add(new VolumeView(BRect(BPoint(0,0), BSize(100.0, 20.0))))
		.AddGlue()
	.End();

	BTabView* tabView = new BTabView("appletTabView");
	tabView->AddTab(new LyricsView(BRect(BPoint(0,0), BSize(100.0, 100.0))));
	tabView->AddTab(new CoverView(BRect(BPoint(0,0), BSize(100.0, 100.0))));
	tabView->AddTab(controlsView);

	BLayoutBuilder::Group<>(this, B_VERTICAL, 0.0f)
		.Add(tabView)
	.End();
}
