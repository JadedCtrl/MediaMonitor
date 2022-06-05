/*
 * Copyright 2022, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef VOLUMEVIEW_H
#define VOLUMEVIEW_H

#include "ReplicantView.h"

class BSlider;


class VolumeView : public ReplicantView {
public:
	                    VolumeView(BRect frame);
	                    VolumeView(BMessage* archive);

	virtual status_t    Archive(BMessage* data, bool deep = true) const;
	static VolumeView*  Instantiate(BMessage* data);

	        void        AttachedToWindow();

	virtual void        MessageReceived(BMessage* msg);
	virtual void        Pulse();

	virtual void        SetInactive(bool inactive);

private:
	        void        _Init();

	        float       _VolumeToPosition(float volume);
	        float       _PositionToVolume(float position);

	BSlider* fSlider;
};

#endif // VOLUMEVIEW_H
