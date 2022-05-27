/*
 * Copyright 2022, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef LYRICSVIEW_H
#define LYRICSVIEW_H

#include <TextView.h>

#include "ReplicantView.h"
#include "Song.h"

class BDragger;
class BPopUpMenu;
class BScrollView;
class MediaPlayer;


enum {
	LYRICS_AUTO_SCROLL = 'lvas',
	LYRICS_TRANSPARENTLY_INACTIVE = 'lvti',
	LYRICS_TRANSPARENTLY_DRAG = 'lvtd'
};


class LyricsTextView : public BTextView {
public:
	                   LyricsTextView(BRect frame, const char* name, BRect textFrame,
	                                  uint32 resize, uint32 flags);
	                   LyricsTextView(BMessage* archive);

	virtual status_t   Archive(BMessage* data, bool deep = true) const;
	static LyricsTextView* Instantiate(BMessage* data);

	virtual void       MouseDown(BPoint where);
};


class LyricsView : public ReplicantView {
public:
	                    LyricsView(BRect frame);
	                    LyricsView(BMessage* archive);

	virtual status_t    Archive(BMessage* data, bool deep = true) const;
	static LyricsView*  Instantiate(BMessage* data);

	virtual void        MessageReceived(BMessage* msg);
	virtual void        Pulse();

	virtual BPopUpMenu* RightClickPopUp(BPopUpMenu* menu = NULL);

	virtual void        SetInactive(bool inactive);

private:
	void                _Init(BRect frame);

	void                _UpdateColors();

	float               _GetPositionProportion();

	Song fCurrentSong;

	LyricsTextView* fTextView;
	BScrollView* fScrollView;

	bool fAutoScroll;

	rgb_color fBgColor;
	rgb_color fFgColor;
};

#endif // LYRICSVIEW_H
