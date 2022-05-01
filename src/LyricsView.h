/*
 * Copyright 2022, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef LYRICSVIEW_H
#define LYRICSVIEW_H

#include <TextView.h>

class BDragger;
class BPopUpMenu;
class BScrollView;


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


class LyricsView : public BView {
public:
	                   LyricsView(BRect frame);
	                   LyricsView(BMessage* archive);

	virtual status_t   Archive(BMessage* data, bool deep = true) const;
	static LyricsView* Instantiate(BMessage* data);

	virtual void       MessageReceived(BMessage* msg);
	virtual void       Pulse();

	virtual void       MouseDown(BPoint where);

private:
	void               _Init(BRect frame);

	void               _SetText(const char* text);
	void               _ClearText();

	void               _UpdateColors();

	BPopUpMenu*        _RightClickPopUp();

	BString            _GetCurrentPath();
	float              _GetPositionProportion();
	int64              _GetIntProperty(const char* specifier);

	LyricsTextView* fTextView;
	BScrollView* fScrollView;
	BDragger* fDragger;

	bool fAutoScroll;
	bool fTransparentInactivity;
	bool fTransparentDragger;

	rgb_color fBgColor;
	rgb_color fFgColor;
	BString fCurrentPath;
};

#endif // LYRICSVIEW_H
