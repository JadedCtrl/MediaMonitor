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
