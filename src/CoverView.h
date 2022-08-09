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
#ifndef COVERVIEW_H
#define COVERVIEW_H

#include "ReplicantView.h"
#include "Song.h"


class CoverView : public ReplicantView {
public:
	                    CoverView(BRect frame);
	                    CoverView(BMessage* archive);

	virtual status_t    Archive(BMessage* data, bool deep = true) const;
	static CoverView*   Instantiate(BMessage* data);

	virtual void        MessageReceived(BMessage* msg);

	virtual void        Pulse();

	virtual BPopUpMenu* RightClickPopUp(BPopUpMenu* menu = NULL);

	virtual void        Draw(BRect updateRect);

private:
	bool                _IsSquare();
	void                _MakeSquare();

	Song fCurrentSong;
	BBitmap* fCover;
};

#endif // COVERVIEW_H
