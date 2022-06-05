/*
 * Copyright 2022, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "App.h"

#include <AboutWindow.h>

#include "MainWindow.h"
#include "Util.h"


App::App()
	:
	BApplication(APP_SIGNATURE)
{
	MainWindow* win = new MainWindow();
	win->SetPulseRate(1250000);
	win->Show();
}


void
App::AboutRequested()
{
	show_about_window(NULL);
}


int
main(int argc, char** argv)
{
	App app;
	app.Run();
	return 0;
}
