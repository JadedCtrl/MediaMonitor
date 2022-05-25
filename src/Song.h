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
	            Song();
	            Song(const char* path, int64 duration = -1);

	bool        InitCheck();

	status_t    Lyrics(BString* buffer);
	BBitmap*    Cover();

	BPath       Path();
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
};

#endif // SONG_H
