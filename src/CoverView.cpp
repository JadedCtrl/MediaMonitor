/*
 * Copyright 2022, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "CoverView.h"

#include <Bitmap.h>
#include <Dragger.h>
#include <Messenger.h>

#include "MediaPlayer.h"


CoverView::CoverView(BRect frame)
	:
	ReplicantView(frame, "Cover", B_FOLLOW_LEFT)
{
	SetViewColor(B_TRANSPARENT_COLOR);
	fCover = NULL;
	Pulse();
}


CoverView::CoverView(BMessage* data)
	:
	ReplicantView(data)
{
	SetViewColor(B_TRANSPARENT_COLOR);
	fCover = NULL;
	Pulse();
}


status_t
CoverView::Archive(BMessage* data, bool deep) const
{
	status_t status = ReplicantView::Archive(data, deep);

	data->AddString("class", "CoverView");
	data->AddString("add_on", APP_SIGNATURE);
	return status;	
}


CoverView*
CoverView::Instantiate(BMessage* data)
{
	if (!validate_instantiation(data, "CoverView"))
		return NULL;
	return new CoverView(data);
}


void
CoverView::Pulse()
{
	Song song;
	// No song playing
	if (fMediaPlayer->CurrentItem(&song, false) == false) {
		if (fCurrentSong.InitCheck()) {
			fCurrentSong = song;
			delete fCover;
			fCover = NULL;

			SetInactive(true);
		}
	// New song
	} else if (song != fCurrentSong) {
		fCurrentSong = song;
		if (fCover != NULL)
			delete fCover;
		fCover = song.Cover();

		SetInactive(false);
	}
}


void
CoverView::Draw(BRect updateRect)
{
	BView::Draw(updateRect);
	if (fCover != NULL && fCover->IsValid())
		DrawBitmap(fCover, Bounds());
}
