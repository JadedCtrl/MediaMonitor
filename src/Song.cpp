/*
 * Copyright 2022, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "Song.h"

#include <iostream>

#include <Directory.h>
#include <File.h>
#include <String.h>


Song::Song(const char* path)
{
	fPath = BPath(path);
}


status_t
Song::Lyrics(BString* buffer)
{
	BFile file(_LyricsPath().Path(), B_READ_ONLY);

	off_t size = 0;
	char* buf = NULL;
	if (file.InitCheck() == B_OK && file.GetSize(&size) == B_OK) {
		buf = (char*)malloc((size_t)size);
		file.Read(buf, size);
		*buffer << buf;
		free(buf);
	} else
		*buffer = "No lyrics available.\n";
	return B_OK;
}


// Our song's leaf, sans file extension
const char*
Song::_FileLeaf()
{
	BString leaf(fPath.Leaf());
	leaf.TruncateChars(leaf.FindLast('.'));
	return leaf.String();
}


// Search for any text-file with the same leaf of the song
BPath
Song::_LyricsPath()
{
	BString leaf(_FileLeaf());
	BPath parentPath;
	if (fPath.GetParent(&parentPath) != B_OK)
		return BPath();

	BNode node;
	BEntry entry;
	BPath entryPath;
	BDirectory parentDir(parentPath.Path());

	while (parentDir.GetNextEntry(&entry) == B_OK) {
		entry.GetPath(&entryPath);
		BString entryLeaf(entryPath.Leaf());

		if (entryLeaf != leaf && entryLeaf.FindFirst(leaf) >= 0) {
			BString mimeType;
			node.SetTo(&entry);
			if (node.ReadAttrString("BEOS:TYPE", &mimeType) == B_OK && mimeType.StartsWith("text/"))
				return entryPath;
		}
	}
	return BPath();
}