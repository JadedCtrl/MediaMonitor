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

#include "MediaPlayer.h"

#include <Messenger.h>
#include <Url.h>

#include "Song.h"


// This seems a fair amount of Pulse()-es to wait before spam-checking windows.
const int32 kTargetHops = 20;


MediaPlayer::MediaPlayer(int32 window, target followType)
{
	fWindowIndex = window;
	fWindowTarget = followType;
	fWindowTargetHops = 0;
}


MediaPlayer::MediaPlayer(BMessage* data)
	:
	BArchivable(data)
{
	fWindowIndex = data->GetInt32("_window", -1);
	fWindowTarget = (target)data->GetInt32("_windowtype", MP_BY_TYPE_AUDIO);
	fWindowTargetHops = 0;
}


status_t
MediaPlayer::Archive(BMessage* data, bool deep) const
{
	status_t status = BArchivable::Archive(data, deep);
	data->AddInt32("_window", fWindowIndex);
	data->AddInt32("_windowtype", fWindowTarget);
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
MediaPlayer::IsValid()
{
	BString type;
	BMessage send, reply;
	_ScriptingCall("Suites", &send, &reply, MP_NO_TRACK, true);
	return reply.FindString("suites", &type) == B_OK && type == "suite/vnd.Haiku-MediaPlayer";
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
MediaPlayer::Window()
{
	if (fWindowTarget == MP_BY_INDEX || fWindowTargetHops-- > 0)
		return fWindowIndex;

	fWindowTargetHops = kTargetHops;

	Song current;
	CurrentItem(&current, false);

	for (int32 i = CountWindows() - 1; i >= 0; i--) {
		MediaPlayer mp(i, MP_BY_INDEX);
		if (mp.IsValid()) {
			if (fWindowTarget == MP_BY_LATEST)
				return fWindowIndex = i;

			Song mpcurrent;
			mp.CurrentItem(&mpcurrent, false);

			// If the current item's type is correct, we'll keep it
			if (!current.InitCheck() || !_MatchesTypeTarget(current.Type(), fWindowTarget))
				if (mpcurrent.InitCheck() && _MatchesTypeTarget(mpcurrent.Type(), fWindowTarget))
					return fWindowIndex = i;
				else
					fWindowIndex = -1;
		}
	}
	return fWindowIndex;
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
	_ScriptingCall("Window", &send, &reply, MP_NO_TRACK, false);

	int32 count;
	if (reply.FindInt32("result", &count) == B_OK)
		return count;
	return -1;
}


target
MediaPlayer::Target()
{
	return fWindowTarget;
}


target
MediaPlayer::SetTarget(target target)
{
	fWindowTargetHops = 0;
	return fWindowTarget = target;
}


bool
MediaPlayer::_GetSong(Song* song, int32 trackIndex, bool durationReq)
{
	BMessage send, reply;
	_ScriptingCall("URI", &send, &reply, trackIndex);
	BString uriString;
	if (reply.FindString("result", &uriString) != B_OK)
		return false;

	int64 duration = -1;
	if (durationReq) {
		_ScriptingCall("Duration", &send, &reply, trackIndex);
		if (reply.FindInt64("result", &duration) != B_OK)
			return false;
	}

	if (trackIndex == MP_CURRENT_TRACK && fWindowTarget != MP_BY_INDEX
		&& uriString != fLastItemPath) {
		fLastItemPath.SetTo(uriString);
		fWindowTargetHops = 0;
	}
	*song = Song(_UriToPath(uriString), duration);
	return true;
}


void
MediaPlayer::_ScriptingCall(const char* attribute, BMessage* send, BMessage* reply, int32 trackIndex,
	bool window)
{
	if (send->what == 0)
	send->what = B_GET_PROPERTY;

	send->AddSpecifier(attribute);
	if (trackIndex > 0)
		send->AddSpecifier("PlaylistTrack", trackIndex);
	else if (trackIndex == MP_CURRENT_TRACK)
		send->AddSpecifier("CurrentTrack");

	if (window)
		send->AddSpecifier("Window", Window());

	if (BMessenger("application/x-vnd.Haiku-MediaPlayer").SendMessage(send, reply) != B_OK)
		return;

	if (reply != NULL && reply->what == B_MESSAGE_NOT_UNDERSTOOD)
		fWindowTargetHops = 0;
}


bool
MediaPlayer::_MatchesTypeTarget(BString mimetype, target typeTarget)
{
	return (mimetype.StartsWith("video") && typeTarget == MP_BY_TYPE_VIDEO)
		|| (mimetype.StartsWith("audio") && typeTarget == MP_BY_TYPE_AUDIO);
}


const char*
MediaPlayer::_UriToPath(BString URI)
{
	if (URI.StartsWith("file://"))
		return URI.ReplaceFirst("file://", "").String();
	else
		return NULL;
}
