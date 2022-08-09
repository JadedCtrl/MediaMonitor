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


enum target {
	MP_BY_INDEX,
	MP_BY_TYPE_AUDIO,
	MP_BY_TYPE_VIDEO,
	MP_BY_LATEST
};


class MediaPlayer : public BArchivable {
public:
	                     MediaPlayer(int32 window = 0, target followType = MP_BY_TYPE_AUDIO);
	                     MediaPlayer(BMessage* archive);

	virtual status_t     Archive(BMessage* data, bool deep = true) const;
	static  MediaPlayer* Instantiate(BMessage* data);

	        bool         IsValid();

	        bool         IsPlaying();
	        int64        Duration();
	        int64        Position();
	        void         SetPosition(int64 position);
	        float        Volume();
	        void         SetVolume(float volume);

	        bool         CurrentItem(Song* song, bool duration = true);
	        bool         PlaylistItem(Song* song, int32 index, bool duration = true);

	        int32        Window();
	        int32        SetWindow(int32 index);
	        int32        CountWindows();

	        target       Target();
	        target       SetTarget(target target);

private:
	        bool         _GetSong(Song* song, int32 trackIndex, bool duration = true);
	        void         _ScriptingCall(const char* attribute, BMessage* send, BMessage* reply,
		                                int32 trackIndex = MP_NO_TRACK, bool window = true);

	        const char*  _UriToPath(BString URI);

	int32 fWindowIndex;
	target fWindowTarget;
	int32 fWindowTargetHops; // Amount of pulses to wait before re-checking windows

	BString fLastItemPath;
};

#endif // MEDIAPLAYER_H
