/*
 * Copyright 2022, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef SONG_H
#define SONG_H

#include <Path.h>


class Song {
public:
	            Song(const char* path);
	   status_t Lyrics(BString* buffer);

private:
	const char* _FileLeaf();
	BPath       _LyricsPath();

	BPath fPath;
};

#endif // SONG_H
