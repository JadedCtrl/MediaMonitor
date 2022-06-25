/*
 * Copyright 2022, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef REPLICANTVIEW_H
#define REPLICANTVIEW_H

#include <View.h>

class BDragger;
class BMenu;
class BPopUpMenu;
class MediaPlayer;


enum {
	REPL_TRANSPARENTLY_INACTIVE = 'rtti',
	REPL_TRANSPARENTLY_DRAG = 'rttd',
	REPL_WIN_BY_INDEX = 'rwbi',
	REPL_WIN_BY_LATEST = 'rwbl',
	REPL_WIN_BY_AUDIO = 'rwba',
	REPL_WIN_BY_VIDEO = 'rwbv'
};


/* Base class for each replicant, containing features need for (most) of them:
	- Dragger where you'd expect it
	- Transparency/hiding during inactivity
	- Right-click menu with common options
	- Setting custom background color (by drag)
	- MediaPlayer object */
class ReplicantView : public BView {
public:
	                       ReplicantView(BRect frame, const char* name,
		                                 uint32 draggerPlacement = B_FOLLOW_RIGHT,
										 uint32 resize = B_FOLLOW_ALL_SIDES,
										 uint32 flags = 0);
	                       ReplicantView(BMessage* archive);

	virtual status_t       Archive(BMessage* data, bool deep = true) const;
	static  ReplicantView* Instantiate(BMessage* data);

	virtual void           MessageReceived(BMessage* msg);

	virtual void           MouseDown(BPoint where);

	        void           Show();

	// Populates the given menu with default items
	virtual BPopUpMenu*    RightClickPopUp(BPopUpMenu* menu = NULL);

	// Set inactivity state, re-render accordingly
	virtual void           SetInactive(bool inactive);

private:
	BMenu* _WindowIndexMenu();

protected:
	MediaPlayer* fMediaPlayer;
	BDragger* fDragger;

	BString fDescription;

	bool fReplicated;
	bool fInactive;

	bool fTransparentInactivity;
	bool fTransparentDragger;
};

#endif // REPLICANTVIEW_H
