/*
 * Copyright 2022, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef SONG_H
#define SONG_H

#include <Path.h>

class BBitmap;

class Song {
public:
	            Song(const char* path);
	   status_t Lyrics(BString* buffer);
	   BBitmap* Cover();

private:
	const char* _FileLeaf();
	BPath       _LyricsPath();
	BPath       _CoverPath();

	BPath       _SimilarFileOfType(const char* mimeRoot);
	BPath       _AnyFileOfType(const char* mimeRoot);
	BPath       _FindFile(const char* mimeRoot, bool byName);

	BPath fPath;
};

#endif // SONG_H
