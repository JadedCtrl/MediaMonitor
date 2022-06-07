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
	data->AddString("add_on", APP_SIGNATURE);
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
	BMessage send, reply;
	_ScriptingCall("IsPlaying", &send, &reply);
	return reply.GetBool("result", false);
}


int64
MediaPlayer::Duration()
{
	BMessage send, reply;
	_ScriptingCall("Duration", &send, &reply, MP_CURRENT_TRACK);
	return reply.GetInt64("result", -1);
}


int64
MediaPlayer::Position()
{
	BMessage send, reply;
	_ScriptingCall("Position", &send, &reply);
	return reply.GetInt64("result", -1);
}


void
MediaPlayer::SetPosition(int64 position)
{
	BMessage send(B_SET_PROPERTY), reply;
	send.AddInt64("data", position);
	_ScriptingCall("Position", &send, &reply);
}


float
MediaPlayer::Volume()
{
	BMessage send, reply;
	_ScriptingCall("Volume", &send, &reply);
	return reply.GetFloat("result", -1.0);
}


void
MediaPlayer::SetVolume(float volume)
{
	BMessage send(B_SET_PROPERTY), reply;
	send.AddFloat("data", volume);
	_ScriptingCall("Volume", &send, &reply);
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
	BMessage send(B_COUNT_PROPERTIES), reply;
	_ScriptingCall("Window", &send, &reply, MP_NO_TRACK);

	int32 count;
	if (reply.FindInt32("result", &count) == B_OK)
		return count;
	return -1;
}


bool
MediaPlayer::_GetSong(Song* song, int32 trackIndex, bool durationRequired)
{
	BMessage send, reply;
	_ScriptingCall("URI", &send, &reply, trackIndex);
	BString uriString;
	if (reply.FindString("result", &uriString) != B_OK)
		return false;

	int64 duration = -1;
	if (durationRequired) {
		_ScriptingCall("Duration", &send, &reply, trackIndex);
		if (reply.FindInt64("result", &duration) != B_OK)
			return false;
	}

	*song = Song(_UriToPath(uriString), duration);
	return true;
}


void
MediaPlayer::_ScriptingCall(const char* attribute, BMessage* send, BMessage* reply, int32 trackIndex)
{
	if (send->what == 0)
	send->what = B_GET_PROPERTY;

	send->AddSpecifier(attribute);
	if (trackIndex > 0)
		send->AddSpecifier("PlaylistTrack", trackIndex);
	else if (trackIndex == MP_CURRENT_TRACK)
		send->AddSpecifier("CurrentTrack");

	send->AddSpecifier("Window", fWindowIndex);

	BMessenger("application/x-vnd.Haiku-MediaPlayer").SendMessage(send, reply);
}


const char*
MediaPlayer::_UriToPath(BString URI)
{
	if (URI.StartsWith("file://"))
		return URI.ReplaceFirst("file://", "").String();
	else
		return NULL;
}
