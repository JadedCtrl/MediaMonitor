/*
 * Copyright 2022, Jaidyn Levesque <jadedctrl@posteo.at>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "MainWindow.h"

#include <LayoutBuilder.h>
#include <SplitView.h>
#include <TabView.h>

#include "CoverView.h"
#include "LyricsView.h"
#include "PositionView.h"
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
		.Add(new PositionView(BRect(BPoint(0,0), BSize(100.0, 20.0))))
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
