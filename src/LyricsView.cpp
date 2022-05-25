/*
 * Copyright 2022, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "LyricsView.h"

#include <Dragger.h>
#include <MenuItem.h>
#include <Messenger.h>
#include <PopUpMenu.h>
#include <ScrollView.h>
#include <Window.h>

#include "MediaPlayer.h"


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
		((LyricsView*)Parent()->Parent())->MouseDown(
			Parent()->ConvertToParent(ConvertToParent(where)));
	else
		BTextView::MouseDown(where);
}


LyricsView::LyricsView(BRect frame)
	:
	BView(frame, "Lyrics", B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_TRANSPARENT_BACKGROUND | B_PULSE_NEEDED)
{
	BRect dragRect(0, 0, 10, frame.Height());
	fDragger = new BDragger(dragRect, this,
		B_FOLLOW_LEFT | B_FOLLOW_BOTTOM, B_WILL_DRAW);
	fDragger->SetViewColor(B_TRANSPARENT_COLOR);
	AddChild(fDragger);

	BRect textRect(0, 0, Bounds().Width(), Bounds().Height() - 10);
	fTextView = new LyricsTextView(textRect, "lyricsText", textRect,
		B_FOLLOW_ALL, B_WILL_DRAW);
	fTextView->MakeEditable(false);
	fTextView->SetStylable(true);

	fScrollView = new BScrollView("scrollView", fTextView, B_FOLLOW_ALL_SIDES,
		B_WILL_DRAW, false, true, B_NO_BORDER);
	fScrollView->ScrollBar(B_VERTICAL)->Hide();
	AddChild(fScrollView);

	fAutoScroll = false;
	fTransparentInactivity = true;
	fTransparentDragger = false;

	fFgColor = ui_color(B_PANEL_TEXT_COLOR);
	fBgColor = ui_color(B_PANEL_BACKGROUND_COLOR);

	fMediaPlayer = new MediaPlayer(0);

	_Init(frame);
}


LyricsView::LyricsView(BMessage* data)
	:
	BView(data)
{
	fAutoScroll = false;
	fTransparentInactivity = true;
	fTransparentDragger = false;

	fFgColor = ui_color(B_PANEL_TEXT_COLOR);
	fBgColor = ui_color(B_PANEL_BACKGROUND_COLOR);

	fTextView = dynamic_cast<LyricsTextView*>(FindView("lyricsText"));
	fScrollView = dynamic_cast<BScrollView*>(FindView("scrollView"));
	fDragger = dynamic_cast<BDragger*>(FindView("_dragger_"));
	data->FindColor("background_color", &fBgColor);
	data->FindColor("foreground_color", &fFgColor);
	data->FindBool("autoscroll", &fAutoScroll);
	data->FindBool("transparent_inactivity", &fTransparentInactivity);
	data->FindBool("transparent_dragger", &fTransparentDragger);

	BMessage mediaplayer;
	data->FindMessage("mediaplayer", &mediaplayer);
	fMediaPlayer = new MediaPlayer(&mediaplayer);

	_Init(Frame());
}


status_t
LyricsView::Archive(BMessage* data, bool deep) const
{
	status_t status = BView::Archive(data, deep);

	BMessage mediaPlayer;
	fMediaPlayer->Archive(&mediaPlayer);
	data->AddMessage("mediaplayer", &mediaPlayer);
	data->AddColor("background_color", fBgColor);
	data->AddColor("foreground_color", fFgColor);
	data->AddBool("autoscroll", fAutoScroll);
	data->AddBool("transparent_inactivity", fTransparentInactivity);
	data->AddBool("transparent_dragger", fTransparentDragger);


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
		case LYRICS_AUTO_SCROLL:
			fAutoScroll = !fAutoScroll;
			break;
		case LYRICS_TRANSPARENTLY_INACTIVE:
		case LYRICS_TRANSPARENTLY_DRAG: {
			if (msg->what == LYRICS_TRANSPARENTLY_INACTIVE)
				fTransparentInactivity = !fTransparentInactivity;
			if (msg->what == LYRICS_TRANSPARENTLY_DRAG)
				fTransparentDragger = !fTransparentDragger;

			if (fCurrentSong.InitCheck() == false)
				_ClearText();
			break;
		}
		default:
			BView::MessageReceived(msg);
			break;
	}
}


void
LyricsView::Pulse()
{
	Song song;
	if (fMediaPlayer->CurrentItem(&song, fAutoScroll) == false) {
		if (fCurrentSong.InitCheck()) {
			fCurrentSong = song;
			_ClearText();
		}
		return;
	}

	if (song != fCurrentSong) {
		fCurrentSong = song;
		BString lyrics;
		song.Lyrics(&lyrics);
		_SetText(lyrics.String());
	}

	if (fAutoScroll) {
		float position = _GetPositionProportion();
		if (position > 0)
			fTextView->ScrollToOffset(fTextView->TextLength() * position);
	}
}


void
LyricsView::MouseDown(BPoint where)
{
	uint32 buttons = 0;
	Window()->CurrentMessage()->FindInt32("buttons", (int32*)&buttons);

	if (buttons & B_SECONDARY_MOUSE_BUTTON)
		_RightClickPopUp()->Go(ConvertToScreen(where), true, false, true);
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
	if (fScrollView->IsHidden())
		fScrollView->Show();
	if (fDragger->IsHidden())
		fDragger->Show();

	fTextView->SetText(text);
	fTextView->SetAlignment(B_ALIGN_LEFT);
}


void
LyricsView::_ClearText()
{
	if (fTransparentInactivity) {
		if (fScrollView->IsHidden())
			fScrollView->Hide();
		if (fDragger->IsHidden() && fTransparentDragger)
			fDragger->Hide();
	} else {
		if (fScrollView->IsHidden())
			fScrollView->Show();
		if (fDragger->IsHidden())
			fDragger->Show();
	}

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


BPopUpMenu*
LyricsView::_RightClickPopUp()
{
	BPopUpMenu* menu = new BPopUpMenu("rightClickPopUp");
	menu->SetRadioMode(false);

	BMenuItem* copy =
		new BMenuItem("Copy", new BMessage(B_COPY), 'C', B_COMMAND_KEY);

	int32 start = -1, end = -1;
	fTextView->GetSelection(&start, &end);

	copy->SetEnabled(start >= 0 && end > 0);
	copy->SetTarget(fTextView);
	menu->AddItem(copy);

	BMenuItem* selectAll = new BMenuItem("Select all",
		new BMessage(B_SELECT_ALL), 'A', B_COMMAND_KEY);
	selectAll->SetTarget(fTextView);
	menu->AddItem(selectAll);

	menu->AddSeparatorItem();

	BMenuItem* autoScroll = new BMenuItem("Auto-scroll",
		new BMessage(LYRICS_AUTO_SCROLL));
	autoScroll->SetMarked(fAutoScroll);
	autoScroll->SetTarget(this);
	menu->AddItem(autoScroll);

	BMenu* hideMenu = new BMenu("Hide when inactive");
	menu->AddItem(hideMenu);

	BMenuItem* hideInactive = hideMenu->Superitem();
	hideInactive->SetMessage(new BMessage(LYRICS_TRANSPARENTLY_INACTIVE));
	hideInactive->SetMarked(fTransparentInactivity);
	hideInactive->SetTarget(this);

	BMenuItem* hideDragger = new BMenuItem("… including the dragger",
		new BMessage(LYRICS_TRANSPARENTLY_DRAG));
	hideDragger->SetMarked(fTransparentDragger);
	hideDragger->SetTarget(this);
	hideMenu->AddItem(hideDragger);

	return menu;
}


float
LyricsView::_GetPositionProportion()
{
	int64 position = fMediaPlayer->Position();
	int64 duration = fCurrentSong.Duration();
	if (position >= 0 && duration > 0)
		return (float)position / (float)duration;
	return -1.0;
}
