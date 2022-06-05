/*
 * Copyright 2022, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "VolumeView.h"

#include <cmath>

#include <Slider.h>

#include "MediaPlayer.h"


// The same color MediaPlayer uses, interface/VolumeSlider
static const rgb_color kVolumeGreen = (rgb_color){ 116, 224, 0, 255 };

static const uint32 VOLUME_CHANGED = 'vvch';


VolumeView::VolumeView(BRect frame)
	:
	ReplicantView(frame, "Volume", B_FOLLOW_RIGHT, B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW)
{
	SetViewColor(B_TRANSPARENT_COLOR);

	fSlider = new BSlider(frame, "volumeSlider", NULL, NULL, 0, 1000, B_HORIZONTAL, B_BLOCK_THUMB,
		B_FOLLOW_LEFT_RIGHT);
	fSlider->SetModificationMessage(new BMessage(VOLUME_CHANGED));
	fSlider->UseFillColor(true, &kVolumeGreen);
	fSlider->SetViewColor(B_TRANSPARENT_COLOR);
	AddChild(fSlider);

	_Init();
}


VolumeView::VolumeView(BMessage* data)
	:
	ReplicantView(data)
{
	fSlider = dynamic_cast<BSlider*>(FindView("volumeSlider"));
	if (fSlider->IsHidden())
		fSlider->Show();
	_Init();
}


status_t
VolumeView::Archive(BMessage* data, bool deep) const
{
	status_t status = ReplicantView::Archive(data, deep);

	data->AddString("class", "VolumeView");
	data->AddString("add_on", APP_SIGNATURE);
	return status;	
}


VolumeView*
VolumeView::Instantiate(BMessage* data)
{
	if (!validate_instantiation(data, "VolumeView"))
		return NULL;
	return new VolumeView(data);
}


void
VolumeView::AttachedToWindow()
{
	fSlider->SetTarget(this);
}


void
VolumeView::MessageReceived(BMessage* msg)
{
	switch (msg->what) {
		case VOLUME_CHANGED:
			fMediaPlayer->SetVolume(_PositionToVolume(fSlider->Position()));
			break;
		case B_MOUSE_WHEEL_CHANGED:
		{
			float scroll = 0.0f;
			if ((msg->FindFloat("be:wheel_delta_x", &scroll) == B_OK && scroll != 0.0f)
				|| (msg->FindFloat("be:wheel_delta_y", &scroll) == B_OK && scroll != 0.0f))
				fSlider->SetPosition(fSlider->Position() + scroll * -0.03);
			fMediaPlayer->SetVolume(_PositionToVolume(fSlider->Position()));
			break;
		}
		default:
			ReplicantView::MessageReceived(msg);
	}
}


void
VolumeView::Pulse()
{
	float volume = fMediaPlayer->Volume();
	if (volume > 0) {
		SetInactive(false);
		fSlider->SetPosition(_VolumeToPosition(volume));
	} else
		SetInactive(true);
}


void
VolumeView::SetInactive(bool inactive)
{
	fSlider->SetEnabled(!inactive);

	if (inactive && fTransparentInactivity && !fSlider->IsHidden())
		fSlider->Hide();
	else if ((!inactive || !fTransparentInactivity) && fSlider->IsHidden())
		fSlider->Show();

	ReplicantView::SetInactive(inactive);
}


void
VolumeView::_Init()
{
	float height = 20.0;
	fSlider->GetPreferredSize(NULL, &height);
	SetExplicitMaxSize(BSize(B_SIZE_UNSET, height + 16));

	Pulse();
}


// The VolumeSlider for MediaPlayer isn't quite linear… I've tried my best to replicate it
// without actually tearing the guts from MediaPlayer or expending too much effort.
// It's close enough, probably…
float
VolumeView::_VolumeToPosition(float volume)
{
	if (volume > 1)
		return volume / 2;
	else if (volume != 0)
		return (-.91 * pow(volume, 2)) + (1.3 * volume) + .06;
	return 0;
}


// … please don't hate me :w:"
float
VolumeView::_PositionToVolume(float position)
{
	if (position > .5)
		return position * 2;
	else if (position != 0) {
		float vol = -130 + pow(-36400 * position + 19084, .5);
		return abs(vol / 182);
	}
	return 0;
}
