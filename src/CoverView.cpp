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

#include "CoverView.h"

#include <Bitmap.h>
#include <MenuItem.h>
#include <PopUpMenu.h>
#include <Window.h>

#include "MediaPlayer.h"


const uint32 COVER_MAKE_SQUARE = 'cvsq';
const char* kCoverDesc = "CoverView is a replicant that shows a thumbnail for whatever is playing― often an album cover taken from a 'cover.jpg' or similar file.";


CoverView::CoverView(BRect frame)
	:
	ReplicantView(frame, "Cover", B_FOLLOW_RIGHT, B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE)
{
	fDescription.SetTo(kCoverDesc);

	SetViewColor(B_TRANSPARENT_COLOR);
	fCover = NULL;
	Pulse();
}


CoverView::CoverView(BMessage* data)
	:
	ReplicantView(data)
{
	fDescription.SetTo(kCoverDesc);

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
CoverView::MessageReceived(BMessage* msg)
{
	if (msg->what == COVER_MAKE_SQUARE)
		_MakeSquare();
	else
		ReplicantView::MessageReceived(msg);
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


BPopUpMenu*
CoverView::RightClickPopUp(BPopUpMenu* menu)
{
	BPopUpMenu* newMenu = ReplicantView::RightClickPopUp(menu);
	newMenu->ItemAt(0)->SetEnabled(false);
	newMenu->ItemAt(0)->SetMarked(false);

	BMenuItem* square = new BMenuItem("Resize to square",
		new BMessage(COVER_MAKE_SQUARE));
	square->SetMarked(_IsSquare());
	square->SetEnabled(!_IsSquare());
	square->SetTarget(this);
	newMenu->AddItem(square, 0);

	return newMenu;
}


void
CoverView::Draw(BRect updateRect)
{
	BView::Draw(updateRect);
	if (fCover != NULL && fCover->IsValid())
		DrawBitmap(fCover, Bounds());
}


bool
CoverView::_IsSquare()
{
	return floorf(Frame().Width()) == floorf(Frame().Height());
}


void
CoverView::_MakeSquare()
{
	if (_IsSquare())
		return;

	ResizeTo(Frame().Height(), Frame().Height());

	// If in MediaMonitor's window, resize the window too.
	// (We don't want to resize Tracker, or other shelves!)
	if (Parent() != NULL && Parent()->Parent() != NULL) {
		BRect pRect = Parent()->Parent()->Frame();
		if (strcmp(Parent()->Parent()->Name(), "appletTabView") == 0
			&& pRect.Width() < Frame().Width() || pRect.Height() < Frame().Height())
			Window()->ResizeBy(Frame().Width() - pRect.Width(),
				Frame().Height() - pRect.Height());
	}
}
