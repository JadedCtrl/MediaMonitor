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


ReplicantView::ReplicantView(BRect frame, const char* name, uint32 draggerPlacement)
	:
	BView(frame, name, B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_TRANSPARENT_BACKGROUND | B_PULSE_NEEDED)
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

	fTransparentInactivity = true;
	fTransparentDragger = false;
	fInactive = true;

	fMediaPlayer = new MediaPlayer(0);
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
		case REPL_TRANSPARENTLY_INACTIVE:
		case REPL_TRANSPARENTLY_DRAG: {
			if (msg->what == REPL_TRANSPARENTLY_INACTIVE)
				fTransparentInactivity = !fTransparentInactivity;
			if (msg->what == REPL_TRANSPARENTLY_DRAG)
				fTransparentDragger = !fTransparentDragger;
			SetInactive(fInactive);
			break;
		}
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

	return menu;
}


void
ReplicantView::SetInactive(bool inactive)
{
	fInactive = inactive;

	if (inactive && fTransparentInactivity)
		if (!fDragger->IsHidden() && fTransparentDragger)
			fDragger->Hide();
	if (fDragger->IsHidden() && (!fTransparentDragger || !fTransparentInactivity))
			fDragger->Show();

	Invalidate();
}
