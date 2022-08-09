/*
 * Copyright 2022, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "ReplicantView.h"

#include <Dragger.h>
#include <MenuItem.h>
#include <PopUpMenu.h>
#include <Window.h>

#include "MediaPlayer.h"
#include "Util.h"


ReplicantView::ReplicantView(BRect frame, const char* name, uint32 draggerPlacement, uint32 resize,
	uint32 flags)
	:
	BView(frame, name, resize,
		B_TRANSPARENT_BACKGROUND | B_DRAW_ON_CHILDREN | B_PULSE_NEEDED | flags)
{
	BRect dragRect(frame.Width() - 10, 0, frame.Width(), frame.Height());
	uint32 dragFollow = B_FOLLOW_RIGHT;
	if (draggerPlacement & _VIEW_LEFT_) {
		dragRect = BRect(0, 0, 10, frame.Height());
		dragFollow = B_FOLLOW_LEFT;
	}
	fDragger = new BDragger(dragRect, this, dragFollow | B_FOLLOW_BOTTOM, B_WILL_DRAW);
	fDragger->SetViewColor(B_TRANSPARENT_COLOR);
	AddChild(fDragger);

	fInactive = true;
	fReplicated = false;

	fTransparentInactivity = false;
	fTransparentDragger = false;

	fMediaPlayer = new MediaPlayer();
}


ReplicantView::ReplicantView(BMessage* data)
	:
	BView(data)
{
	BMessage mediaplayer;
	data->FindMessage("mediaplayer", &mediaplayer);
	fMediaPlayer = new MediaPlayer(&mediaplayer);

	fDragger = dynamic_cast<BDragger*>(FindView("_dragger_"));

	fTransparentInactivity = data->GetBool("transparent_inactivity", true);
	fTransparentDragger = data->GetBool("transparent_dragger", false);

	fInactive = true;
	fReplicated = true;
}


status_t
ReplicantView::Archive(BMessage* data, bool deep) const
{
	status_t status = BView::Archive(data, deep);

	BMessage mediaPlayer;
	fMediaPlayer->Archive(&mediaPlayer);
	data->AddMessage("mediaplayer", &mediaPlayer);
	data->AddBool("transparent_inactivity", fTransparentInactivity);
	data->AddBool("transparent_dragger", fTransparentDragger);

	data->AddString("class", "ReplicantView");
	data->AddString("add_on", APP_SIGNATURE);
	return status;
}


ReplicantView*
ReplicantView::Instantiate(BMessage* data)
{
	if (!validate_instantiation(data, "ReplicantView"))
		return NULL;
	return new ReplicantView(data);
}


void
ReplicantView::MessageReceived(BMessage* msg)
{
	switch (msg->what)
	{
		case REPL_WIN_BY_AUDIO:
			fMediaPlayer->SetTarget(MP_BY_TYPE_AUDIO);
			break;
		case REPL_WIN_BY_VIDEO:
			fMediaPlayer->SetTarget(MP_BY_TYPE_VIDEO);
			break;
		case REPL_WIN_BY_LATEST:
			fMediaPlayer->SetTarget(MP_BY_LATEST);
			break;
		case REPL_WIN_BY_INDEX:
		{
			int32 index = 0;
			if (msg->FindInt32("index", &index) == B_OK) {
				fMediaPlayer->SetTarget(MP_BY_INDEX);
				fMediaPlayer->SetWindow(index);
			}
			break;
		}

		case REPL_TRANSPARENTLY_INACTIVE:
		case REPL_TRANSPARENTLY_DRAG: {
			if (msg->what == REPL_TRANSPARENTLY_INACTIVE)
				fTransparentInactivity = !fTransparentInactivity;
			if (msg->what == REPL_TRANSPARENTLY_DRAG)
				fTransparentDragger = !fTransparentDragger;
			SetInactive(fInactive);
			break;
		}
		case B_ABOUT_REQUESTED:
			show_about_window(fDescription.String());
			break;
		default:
			BView::MessageReceived(msg);
			break;
	}
}


void
ReplicantView::MouseDown(BPoint where)
{
	uint32 buttons = 0;
	Window()->CurrentMessage()->FindInt32("buttons", (int32*)&buttons);
	if (buttons & B_SECONDARY_MOUSE_BUTTON)
		RightClickPopUp()->Go(ConvertToScreen(where), true, false, true);
}


void
ReplicantView::Show()
{
	BView::Show();
	if (!IsHidden())
		SetInactive(fInactive);
}


BPopUpMenu*
ReplicantView::RightClickPopUp(BPopUpMenu* menu)
{
	if (menu == NULL)
		menu = new BPopUpMenu("rightClickPopUp");
	menu->SetRadioMode(false);

	BMenu* hideMenu = new BMenu("Hide when inactive");
	menu->AddItem(hideMenu);
	BMenuItem* hideInactive = hideMenu->Superitem();
	hideInactive->SetMessage(new BMessage(REPL_TRANSPARENTLY_INACTIVE));
	hideInactive->SetMarked(fTransparentInactivity);
	hideInactive->SetTarget(this);

	BMenuItem* hideDragger = new BMenuItem("… including the dragger",
		new BMessage(REPL_TRANSPARENTLY_DRAG));
	hideDragger->SetMarked(fTransparentDragger);
	hideDragger->SetTarget(this);
	hideMenu->AddItem(hideDragger);

	BMenu* windowMenu = new BMenu("Target window…");
	menu->AddItem(windowMenu);

	BMenu* indexMenu = _WindowIndexMenu();
	indexMenu->SetTargetForItems(this);
	windowMenu->AddItem(indexMenu);

	BMenu* typeMenu = new BMenu("By type…");
	windowMenu->AddItem(typeMenu);

	BMenuItem* audioItem = new BMenuItem("Audio", new BMessage(REPL_WIN_BY_AUDIO));
	BMenuItem* videoItem = new BMenuItem("Video", new BMessage(REPL_WIN_BY_VIDEO));
	audioItem->SetMarked(fMediaPlayer->Target() == MP_BY_TYPE_AUDIO);
	videoItem->SetMarked(fMediaPlayer->Target() == MP_BY_TYPE_VIDEO);
	typeMenu->AddItem(audioItem);
	typeMenu->AddItem(videoItem);
	typeMenu->SetTargetForItems(this);

	menu->AddSeparatorItem();

	BString aboutLabel = "About %replicant" B_UTF8_ELLIPSIS;
	aboutLabel.ReplaceAll("%replicant", Name());
	BMenuItem* aboutItem = new BMenuItem(aboutLabel, new BMessage(B_ABOUT_REQUESTED));
	aboutItem->SetTarget(this);
	menu->AddItem(aboutItem);

	if (fReplicated) {
		BMenuItem* removeItem = new BMenuItem("Remove replicant", new BMessage(B_TRASH_TARGET));
		removeItem->SetTarget(fDragger);
		menu->AddItem(removeItem);
	}

	return menu;
}


void
ReplicantView::SetInactive(bool inactive)
{
	fInactive = inactive;

	if (IsHidden())
		return;

	if (inactive && fTransparentInactivity)
		if (!fDragger->IsHidden() && fTransparentDragger)
			fDragger->Hide();
	if (fDragger->IsHidden() && (!inactive || !fTransparentDragger || !fTransparentInactivity))
			fDragger->Show();

	Invalidate();
}


BMenu*
ReplicantView::_WindowIndexMenu()
{
	BMenu* menu = new BMenu("By index…");

	BMenuItem* latestItem = new BMenuItem("Latest", new BMessage(REPL_WIN_BY_LATEST));
	latestItem->SetMarked(fMediaPlayer->Target() == MP_BY_LATEST);
	menu->AddItem(latestItem);

	int32 windowCount = fMediaPlayer->CountWindows();
	for (int32 i = 0; i < windowCount; i++)
		if (MediaPlayer(i, MP_BY_INDEX).IsValid()) {
			BString label("");
			label << i;

			BMessage* msg = new BMessage(REPL_WIN_BY_INDEX);
			msg->AddInt32("index", i);

			BMenuItem* indexItem = new BMenuItem(label, msg);
			indexItem->SetMarked(fMediaPlayer->Target() == MP_BY_INDEX
				&& fMediaPlayer->Window() == i);
			menu->AddItem(indexItem);
		}

	return menu;
}
