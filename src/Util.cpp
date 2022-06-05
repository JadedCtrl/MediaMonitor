/*
 * Copyright 2022, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "Util.h"

#include <AboutWindow.h>


void
show_about_window(const char* description)
{
	const char* authors[] = {
		"Stephan Aßmus",
		"DarkWyrm",
		"Jaidyn Levesque",
		NULL
	};

	BAboutWindow* about = new BAboutWindow(APP_NAME, APP_SIGNATURE);
	about->AddDescription("A collection of simple MediaPlayer replicants.");
	if (description != NULL)
		about->AddDescription(description);
	about->AddAuthors(authors);
	about->AddCopyright(2022, "Jaidyn Levesque");
	about->Show();
}
