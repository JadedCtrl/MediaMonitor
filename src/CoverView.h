/*
 * Copyright 2022, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
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
