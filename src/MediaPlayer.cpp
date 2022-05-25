/*
 * Copyright 2022, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "MediaPlayer.h"

#include <Messenger.h>
#include <Url.h>

#include "Song.h"


MediaPlayer::MediaPlayer(int32 window)
{
	fWindowIndex = window;
}


MediaPlayer::MediaPlayer(BMessage* data)
	:
	BArchivable(data)
{
	fWindowIndex = data->GetInt32("_window", 0);
}


status_t
MediaPlayer::Archive(BMessage* data, bool deep) const
{
	status_t status = BArchivable::Archive(data, deep);
	data->AddInt32("_window", fWindowIndex);
	data->AddString("class", "MediaPlayer");
	data->AddString("add_on", "application/x-vnd.mediamonitor");
	return status;
}


MediaPlayer*
MediaPlayer::Instantiate(BMessage* data)
{
	if (!validate_instantiation(data, "MediaPlayer"))
		return NULL;
	return new MediaPlayer(data);
}


bool
MediaPlayer::IsPlaying()
{
	BMessage reply;
	_GetResponse("IsPlaying", &reply);
	return reply.GetBool("result", false);
}


int64
MediaPlayer::Position()
{
	BMessage reply;
	_GetResponse("Position", &reply);
	return reply.GetInt64("result", -1);
}


bool
MediaPlayer::CurrentItem(Song* song, bool duration)
{
	return _GetSong(song, MP_CURRENT_TRACK, duration);
}


bool
MediaPlayer::PlaylistItem(Song* song, int32 index, bool duration)
{
	return _GetSong(song, index, duration);
}


int32
MediaPlayer::SetWindow(int32 index)
{
	if (index < CountWindows())
		return fWindowIndex = index;
	else
		return -1;
}


int32
MediaPlayer::CountWindows()
{
	BMessage reply;
	_GetResponse("Window", &reply, MP_NO_TRACK, B_COUNT_PROPERTIES);

	int32 count;
	if (reply.FindInt32("result", &count) == B_OK)
		return count;
	return -1;
}


bool
MediaPlayer::_GetSong(Song* song, int32 trackIndex, bool durationRequired)
{
	BMessage reply;
	_GetResponse("URI", &reply, trackIndex);
	BString uriString;
	if (reply.FindString("result", &uriString) != B_OK)
		return false;

	int64 duration = -1;
	if (durationRequired) {
		_GetResponse("Duration", &reply, trackIndex);
		if (reply.FindInt64("result", &duration) != B_OK)
			return false;
	}

	*song = Song(_UriToPath(uriString), duration);
	return true;
}


void
MediaPlayer::_GetResponse(const char* attribute, BMessage* reply, int32 trackIndex, int32 what)
{
	BMessage message;
	message.what = what;
	message.AddSpecifier(attribute);
	if (trackIndex > 0)
		message.AddSpecifier("PlaylistTrack", trackIndex);
	else if (trackIndex == MP_CURRENT_TRACK)
		message.AddSpecifier("CurrentTrack");
	message.AddSpecifier("Window", fWindowIndex);
	BMessenger("application/x-vnd.Haiku-MediaPlayer").SendMessage(&message, reply);
}


const char*
MediaPlayer::_UriToPath(BString URI)
{
	if (URI.StartsWith("file://"))
		return URI.ReplaceFirst("file://", "").String();
	else
		return NULL;
}
