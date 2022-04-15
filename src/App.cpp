/*
 * Copyright 2022, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "App.h"

#include <iostream>

#include "MainWindow.h"


BString
get_string_mediaplayer(const char* attribute)
{
	BMessage message, reply;
	message.what = B_GET_PROPERTY;
	message.AddSpecifier(attribute);
	message.AddSpecifier("Window", 0);
	BMessenger("application/x-vnd.Haiku-MediaPlayer").SendMessage(&message, &reply);

	BString result;
	reply.FindString("result", &result);
	return result;
}


App::App()
	:
	BApplication("application/x-vnd.mediamonitor")
{
	MainWindow* win = new MainWindow();
	win->SetPulseRate(5000000);
	win->Show();
}


int
main(int argc, char** argv)
{
	App app;
	app.Run();
	return 0;
}
