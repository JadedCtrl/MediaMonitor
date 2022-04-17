/*
 * Copyright 2022, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "LyricsView.h"

#include <cstdio>

#include <Dragger.h>
#include <Messenger.h>
#include <ScrollView.h>
#include <TextView.h>

#include "Song.h"


LyricsView::LyricsView(BRect frame)
	:
	BView(frame, "Lyrics", B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_TRANSPARENT_BACKGROUND | B_PULSE_NEEDED)
{
	fFgColor = ui_color(B_PANEL_TEXT_COLOR);
	fBgColor = ui_color(B_PANEL_BACKGROUND_COLOR);

	BRect dragRect(0, 0, 10, frame.Height());
	BDragger* airdrag = new BDragger(dragRect, this,
		B_FOLLOW_LEFT | B_FOLLOW_BOTTOM, B_WILL_DRAW);
	airdrag->SetViewColor(B_TRANSPARENT_COLOR);
	AddChild(airdrag);

	BRect textRect(0, 0, Bounds().Width(), Bounds().Height() - 10);
	fTextView = new BTextView(textRect, "lyricsText", textRect,
		B_FOLLOW_ALL, B_WILL_DRAW);
	fTextView->MakeEditable(false);
	fTextView->SetStylable(false);

	fScrollView = new BScrollView("scrollView", fTextView, B_FOLLOW_ALL_SIDES,
		B_WILL_DRAW, false, true, B_NO_BORDER);
	fScrollView->ScrollBar(B_VERTICAL)->Hide();
	AddChild(fScrollView);

	_Init(frame);
}


LyricsView::LyricsView(BMessage* data)
	:
	BView(data)
{
	fFgColor = ui_color(B_PANEL_TEXT_COLOR);
	fBgColor = ui_color(B_PANEL_BACKGROUND_COLOR);

	fTextView = dynamic_cast<BTextView*>(FindView("lyricsText"));
	fScrollView = dynamic_cast<BScrollView*>(FindView("scrollView"));
	data->FindColor("background_color", &fBgColor);
	data->FindColor("foreground_color", &fFgColor);

	_Init(Frame());
}


status_t
LyricsView::Archive(BMessage* data, bool deep) const
{
	status_t status = BView::Archive(data, deep);
	data->AddColor("background_color", fBgColor);
	data->AddColor("foreground_color", fFgColor);

	data->AddString("class", "LyricsView");
	data->AddString("add_on", "application/x-vnd.mediamonitor");
	return status;	
}


LyricsView*
LyricsView::Instantiate(BMessage* data)
{
	if (!validate_instantiation(data, "LyricsView"))
		return NULL;
	return new LyricsView(data);
}


void
LyricsView::MessageReceived(BMessage* msg)
{
	switch (msg->what)
	{
		case B_PASTE: {
			ssize_t size;
			int32 buttons = 0;
			rgb_color* recv_color;
			status_t found_color = msg->FindData("RGBColor", B_RGB_COLOR_TYPE,
				(const void**)&recv_color, &size);

			if (found_color == B_OK && msg->FindInt32("buttons", &buttons) == B_OK)
				if (buttons & B_PRIMARY_MOUSE_BUTTON)
					fBgColor = *recv_color;
				else if (buttons & B_SECONDARY_MOUSE_BUTTON)
					fFgColor = *recv_color;

			_UpdateColors();
			break;
		}
	}
	BView::MessageReceived(msg);
}


void
LyricsView::Pulse()
{
	BString path = _GetCurrentPath();
	if (path.IsEmpty() == false && path != fCurrentPath) {
		fCurrentPath = path;
		Song song(path.String());
		BString lyrics;
		song.Lyrics(&lyrics);
		_SetText(lyrics.String());
	} else if (path.IsEmpty() == true && path != fCurrentPath) {
		fCurrentPath = path;
		_SetText(NULL);
	}
}


void
LyricsView::_Init(BRect frame)
{
	_SetText(NULL);
	_UpdateColors();
	Pulse();
}


void
LyricsView::_SetText(const char* text)
{
	if (text != NULL) {
		if (fTextView->IsHidden() == true)
			fTextView->Show();
		fTextView->SetText(text);
	} else if (text == NULL) {
		if (fTextView->IsHidden() == false)
			fTextView->Hide();
		fTextView->SetText("");
	}
}


void
LyricsView::_UpdateColors()
{
	SetViewColor(B_TRANSPARENT_COLOR);
	fScrollView->SetViewColor(B_TRANSPARENT_COLOR);
	fTextView->SetViewColor(fBgColor);

	text_run run;
	run.color = fFgColor;
	run.font = be_plain_font;
	run.offset = 0;

	text_run_array run_array;
	run_array.count = 1;
	run_array.runs[0] = run;
	fTextView->SetRunArray(0, fTextView->TextLength(), &run_array);

	Invalidate();
	fTextView->Invalidate();
}


BString
LyricsView::_GetCurrentPath()
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
