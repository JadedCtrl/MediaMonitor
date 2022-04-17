/*
 * Copyright 2022, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef LYRICSVIEW_H
#define LYRICSVIEW_H

#include <View.h>

class BScrollView;
class BTextView;


class LyricsView : public BView {
public:
	                   LyricsView(BRect frame);
	                   LyricsView(BMessage* archive);

	virtual status_t   Archive(BMessage* data, bool deep = true) const;
	static LyricsView* Instantiate(BMessage* data);

	virtual void       MessageReceived(BMessage* msg);

	virtual void       Pulse();

private:
	void               _Init(BRect frame);

	void               _InitInterface();
	void               _SetText(const char* text);
	void               _UpdateColors();

	BString            _GetCurrentPath();

	BTextView* fTextView;
	BScrollView* fScrollView;

	rgb_color fBgColor;
	rgb_color fFgColor;
	BString fCurrentPath;
};

#endif // LYRICSVIEW_H
