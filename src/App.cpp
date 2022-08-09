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
