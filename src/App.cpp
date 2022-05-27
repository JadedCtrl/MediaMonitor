/*
 * Copyright 2022, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "App.h"

#include "MainWindow.h"


App::App()
	:
	BApplication(APP_SIGNATURE)
{
	MainWindow* win = new MainWindow();
	win->SetPulseRate(2500000);
	win->Show();
}


int
main(int argc, char** argv)
{
	App app;
	app.Run();
	return 0;
}
