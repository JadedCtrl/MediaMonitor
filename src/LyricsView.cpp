/*
 * Copyright 2022, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "LyricsView.h"

#include <cstdio>

#include <Dragger.h>
#include <MenuItem.h>
#include <Messenger.h>
#include <PopUpMenu.h>
#include <ScrollView.h>
#include <Window.h>

#include "Song.h"


LyricsTextView::LyricsTextView(BRect frame, const char* name, BRect textFrame,
	uint32 resize, uint32 flags)
	:
	BTextView(frame, name, textFrame, resize, flags)
{
}


LyricsTextView::LyricsTextView(BMessage* data)
	:
	BTextView(data)
{
}


status_t
LyricsTextView::Archive(BMessage* data, bool deep) const
{
	status_t status = BTextView::Archive(data, deep);
	data->AddString("class", "LyricsTextView");
	data->AddString("add_on", "application/x-vnd.mediamonitor");
	return status;
}


LyricsTextView*
LyricsTextView::Instantiate(BMessage* data)
{
	if (!validate_instantiation(data, "LyricsTextView"))
		return NULL;
	return new LyricsTextView(data);
}


void
LyricsTextView::MouseDown(BPoint where)
{
	uint32 buttons = 0;
	Window()->CurrentMessage()->FindInt32("buttons", (int32*)&buttons);

	if (buttons & B_SECONDARY_MOUSE_BUTTON)
		_RightClickPopUp(where)->Go(ConvertToScreen(where), true, false);
	else
		BTextView::MouseDown(where);
}


BPopUpMenu*
LyricsTextView::_RightClickPopUp(BPoint where)
{
	BPopUpMenu* menu = new BPopUpMenu("rightClickPopUp");
	BMenuItem* copy =
		new BMenuItem("Copy", new BMessage(B_COPY), 'C', B_COMMAND_KEY);
	BMenuItem* selectAll = new BMenuItem("Select all",
		new BMessage(B_SELECT_ALL), 'A', B_COMMAND_KEY);

	int32 start = -1, end = -1;
	GetSelection(&start, &end);

	copy->SetEnabled(start >= 0 && end > 0);
	copy->SetTarget(this);
	menu->AddItem(copy);

	selectAll->SetTarget(this);
	menu->AddItem(selectAll);

	return menu;
}


LyricsView::LyricsView(BRect frame)
	:
	BView(frame, "Lyrics", B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_TRANSPARENT_BACKGROUND | B_PULSE_NEEDED)
{
	BRect dragRect(0, 0, 10, frame.Height());
	BDragger* airdrag = new BDragger(dragRect, this,
		B_FOLLOW_LEFT | B_FOLLOW_BOTTOM, B_WILL_DRAW);
	airdrag->SetViewColor(B_TRANSPARENT_COLOR);
	AddChild(airdrag);

	BRect textRect(0, 0, Bounds().Width(), Bounds().Height() - 10);
	fTextView = new LyricsTextView(textRect, "lyricsText", textRect,
		B_FOLLOW_ALL, B_WILL_DRAW);
	fTextView->MakeEditable(false);
	fTextView->SetStylable(true);

	fScrollView = new BScrollView("scrollView", fTextView, B_FOLLOW_ALL_SIDES,
		B_WILL_DRAW, false, true, B_NO_BORDER);
	fScrollView->ScrollBar(B_VERTICAL)->Hide();
	AddChild(fScrollView);

	fTransparentInactivity = false;
	fFgColor = ui_color(B_PANEL_TEXT_COLOR);
	fBgColor = ui_color(B_PANEL_BACKGROUND_COLOR);

	_Init(frame);
}


LyricsView::LyricsView(BMessage* data)
	:
	BView(data)
{
	fTransparentInactivity = false;
	fFgColor = ui_color(B_PANEL_TEXT_COLOR);
	fBgColor = ui_color(B_PANEL_BACKGROUND_COLOR);

	fTextView = dynamic_cast<LyricsTextView*>(FindView("lyricsText"));
	fScrollView = dynamic_cast<BScrollView*>(FindView("scrollView"));
	data->FindColor("background_color", &fBgColor);
	data->FindColor("foreground_color", &fFgColor);
	data->FindBool("transparent_inactivity", &fTransparentInactivity);

	_Init(Frame());
}


status_t
LyricsView::Archive(BMessage* data, bool deep) const
{
	status_t status = BView::Archive(data, deep);
	data->AddColor("background_color", fBgColor);
	data->AddColor("foreground_color", fFgColor);
	data->AddBool("transparent_inactivity", fTransparentInactivity);

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
		_ClearText();
	}
}


void
LyricsView::_Init(BRect frame)
{
	_ClearText();
	_UpdateColors();
	Pulse();
}


void
LyricsView::_SetText(const char* text)
{
	if (fScrollView->IsHidden() == true)
		fScrollView->Show();

	fTextView->SetText(text);
	fTextView->SetAlignment(B_ALIGN_LEFT);
}


void
LyricsView::_ClearText()
{
	if (fScrollView->IsHidden() == false && fTransparentInactivity == true)
		fScrollView->Hide();

	fTextView->SetText("No lyrics to display!");
	fTextView->SetAlignment(B_ALIGN_CENTER);
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
