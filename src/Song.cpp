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

#include "Song.h"

#include <Directory.h>
#include <File.h>
#include <String.h>
#include <TranslationUtils.h>

#include "thirdparty/TextFile.h"


Song::Song()
{
}


Song::Song(const char* path, int64 duration)
{
	fPath = BPath(path);
	fDuration = duration;
}


bool
Song::InitCheck()
{
	return fPath.InitCheck() == B_OK && fDuration >= -1;
}


status_t
Song::Lyrics(BString* buffer)
{
	TextFile file(_LyricsPath().Path(), B_READ_ONLY);

	if (file.InitCheck() == B_OK) {
		const char* line = file.ReadLine();
		while (line != NULL) {
			*buffer << line << '\n';
			line = file.ReadLine();
		}
	}

	return file.InitCheck();
}


BBitmap*
Song::Cover()
{
	return BTranslationUtils::GetBitmapFile(_CoverPath().Path());
}


BPath
Song::Path()
{
	return fPath;
}


int64
Song::Duration()
{
	return fDuration;
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
	return _SimilarFileOfType("text/");
}


// Search for any image-file in the same directory
BPath
Song::_CoverPath()
{
	BPath path = _SimilarFileOfType("image/");
	if (path.InitCheck() != B_OK)
		path = _AnyFileOfType("image/");
	return path;
}


BPath
Song::_SimilarFileOfType(const char* mimeRoot)
{
	return _FindFile(mimeRoot, true);
}


BPath
Song::_AnyFileOfType(const char* mimeRoot)
{
	return _FindFile(mimeRoot, false);
}


BPath
Song::_FindFile(const char* mimeRoot, bool byName)
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

		bool nameAgrees = (entryLeaf != leaf && entryLeaf.FindFirst(leaf) >= 0);

		if (byName == false || nameAgrees == true) {
			BString mimeType;
			node.SetTo(&entry);
			if (mimeRoot == NULL ||
					(node.ReadAttrString("BEOS:TYPE", &mimeType) == B_OK
					 && mimeType.StartsWith(mimeRoot)))
				return entryPath;
		}
	}
	return BPath();
}
