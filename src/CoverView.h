/*
 * Copyright 2022, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef COVERVIEW_H
#define COVERVIEW_H

#include <View.h>

#include "Song.h"

class BDragger;
class MediaPlayer;


class CoverView : public BView {
public:
	                   CoverView(BRect frame);
	                   CoverView(BMessage* archive);

	virtual status_t   Archive(BMessage* data, bool deep = true) const;
	static CoverView*  Instantiate(BMessage* data);

	virtual void       Pulse();

	virtual void       Draw(BRect updateRect);

private:
	void               _Init(BRect frame);

	MediaPlayer* fMediaPlayer;
	Song fCurrentSong;

	BDragger* fDragger;
	BBitmap* fCover;
};

#endif // COVERVIEW_H
