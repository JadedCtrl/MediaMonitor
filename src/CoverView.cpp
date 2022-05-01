/*
 * Copyright 2022, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "CoverView.h"

#include <Bitmap.h>
#include <Dragger.h>
#include <Messenger.h>

#include "Song.h"


CoverView::CoverView(BRect frame)
	:
	BView(frame, "Cover", B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_TRANSPARENT_BACKGROUND | B_PULSE_NEEDED)
{
	BRect dragRect(0, 0, 10, frame.Height());
	fDragger = new BDragger(dragRect, this,
		B_FOLLOW_LEFT | B_FOLLOW_BOTTOM, B_WILL_DRAW);
	fDragger->SetViewColor(B_TRANSPARENT_COLOR);
	AddChild(fDragger);

	fCover = NULL;
	SetViewColor(B_TRANSPARENT_COLOR);
	Pulse();
}


CoverView::CoverView(BMessage* data)
	:
	BView(data)
{
	fCover = NULL;
	SetViewColor(B_TRANSPARENT_COLOR);
	Pulse();
}


status_t
CoverView::Archive(BMessage* data, bool deep) const
{
	status_t status = BView::Archive(data, deep);
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
	BString path = _GetCurrentPath();
	if (path.IsEmpty() == false && path != fCurrentPath) {
		fCurrentPath = path;
		Song song(path.String());
		delete fCover;
		fCover = song.Cover();
		Invalidate();
	} else if (path.IsEmpty() == true && path != fCurrentPath) {
		fCurrentPath = path;
		delete fCover;
		Invalidate();
	}
}


void
CoverView::Draw(BRect updateRect)
{
	BView::Draw(updateRect);

	if (fCover != NULL)
		DrawBitmap(fCover, Bounds());
}


BString
CoverView::_GetCurrentPath()
{
	BMessage message, reply;
	message.what = B_GET_PROPERTY;
	message.AddSpecifier("URI");
	message.AddSpecifier("Window", 0);
	BMessenger("application/x-vnd.Haiku-MediaPlayer").SendMessage(&message, &reply);

	BString result;
	reply.FindString("result", &result);
	return result.ReplaceAll("file://", "");
}
