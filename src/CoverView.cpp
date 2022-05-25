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
	BView(frame, "Cover", B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_TRANSPARENT_BACKGROUND | B_PULSE_NEEDED)
{
	BRect dragRect(0, 0, 10, frame.Height());
	fDragger = new BDragger(dragRect, this,
		B_FOLLOW_LEFT | B_FOLLOW_BOTTOM, B_WILL_DRAW);
	fDragger->SetViewColor(B_TRANSPARENT_COLOR);
	AddChild(fDragger);

	fMediaPlayer = new MediaPlayer(0);

	fCover = NULL;
	SetViewColor(B_TRANSPARENT_COLOR);
	Pulse();
}


CoverView::CoverView(BMessage* data)
	:
	BView(data)
{
	BMessage mediaplayer;
	data->FindMessage("mediaplayer", &mediaplayer);
	fMediaPlayer = new MediaPlayer(&mediaplayer);

	fCover = NULL;
	SetViewColor(B_TRANSPARENT_COLOR);
	Pulse();
}


status_t
CoverView::Archive(BMessage* data, bool deep) const
{
	status_t status = BView::Archive(data, deep);

	BMessage mediaPlayer;
	fMediaPlayer->Archive(&mediaPlayer);
	data->AddMessage("mediaplayer", &mediaPlayer);
	data->AddString("class", "CoverView");
	data->AddString("add_on", "application/x-vnd.mediamonitor");
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
	if (fMediaPlayer->CurrentItem(&song, false) == false) {
		if (fCurrentSong.InitCheck()) {
			fCurrentSong = song;
			delete fCover;
			Invalidate();
		}
	} else if (song != fCurrentSong) {
		fCurrentSong = song;
		delete fCover;
		fCover = song.Cover();
		Invalidate();
	}
}


void
CoverView::Draw(BRect updateRect)
{
	BView::Draw(updateRect);

	if (fCover != NULL && fCover->IsValid())
		DrawBitmap(fCover, Bounds());
}
