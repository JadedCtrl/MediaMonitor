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
#ifndef SONG_H
#define SONG_H

#include <Path.h>
#include <String.h>

class BBitmap;

class Song {
public:
	            Song();
	            Song(const char* path, int64 duration = -1);

	bool        InitCheck();

	status_t    Lyrics(BString* buffer);
	BBitmap*    Cover();

	BPath       Path();
	BString     Type();
	int64       Duration();

	int operator ==(Song a)
	{
		if (a.Path() == fPath)
			return 1;
		return 0;
	}


	int operator !=(Song a)
	{
		if (a.Path() != fPath)
			return 1;
		return 0;
	}

private:
	const char* _FileLeaf();
	BPath       _LyricsPath();
	BPath       _CoverPath();

	BPath       _SimilarFileOfType(const char* mimeRoot);
	BPath       _AnyFileOfType(const char* mimeRoot);
	BPath       _FindFile(const char* mimeRoot, bool byName);

	int64 fDuration;
	BPath fPath;
	BString fType;
};

#endif // SONG_H
