/*
 * Copyright 2022, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

#include <AppDefs.h>
#include <Archivable.h>
#include <String.h>

class BMessage;
class Song;


enum {
	MP_CURRENT_TRACK = -1,
	MP_NO_TRACK = -2
};


class MediaPlayer : public BArchivable {
public:
	                     MediaPlayer(int32 window = 0);
	                     MediaPlayer(BMessage* archive);

	virtual status_t     Archive(BMessage* data, bool deep = true) const;
	static  MediaPlayer* Instantiate(BMessage* data);


	        bool         IsPlaying();
	        int64        Position();
	        bool         CurrentItem(Song* song, bool duration = true);
	        bool         PlaylistItem(Song* song, int32 index, bool duration = true);

	        int32        SetWindow(int32 index);
	        int32        CountWindows();

private:
	        bool         _GetSong(Song* song, int32 trackIndex, bool durationRequired = true);
	        void         _GetResponse(const char* attribute, BMessage* reply,
		                              int32 trackIndex = MP_NO_TRACK, int32 what = B_GET_PROPERTY);
	        const char*  _UriToPath(BString URI);

	int32 fWindowIndex;
};

#endif // MEDIAPLAYER_H
