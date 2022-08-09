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

#include "VolumeView.h"

#include <cmath>

#include <Slider.h>

#include "MediaPlayer.h"


const char* kVolumeDesc = "VolumeView is a replicant that can monitor and change the volume of MediaPlayer.";


// The same color MediaPlayer uses, interface/VolumeSlider
static const rgb_color kVolumeGreen = (rgb_color){ 116, 224, 0, 255 };

static const uint32 VOLUME_CHANGED = 'vvch';


VolumeView::VolumeView(BRect frame)
	:
	ReplicantView(frame, "Volume", B_FOLLOW_RIGHT, B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW | B_NAVIGABLE | B_FRAME_EVENTS)
{
	fDescription.SetTo(kVolumeDesc);

	_InitInterface();
	Pulse();
}


VolumeView::VolumeView(BMessage* data)
	:
	ReplicantView(data)
{
	fDescription.SetTo(kVolumeDesc);

	// For some reason, the BSlider gets archived with a wacko frame― better to just nuke it.
	fSlider = dynamic_cast<BSlider*>(FindView("volumeSlider"));
	delete fSlider;

	_InitInterface();
	Pulse();
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
			if (fInactive)
				break;

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
	if (volume >= 0) {
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
VolumeView::_InitInterface()
{
	SetViewColor(B_TRANSPARENT_COLOR);

	fSlider = new BSlider(BRect(), "volumeSlider", NULL, NULL, 0, 1000, B_HORIZONTAL, B_BLOCK_THUMB,
		B_FOLLOW_LEFT_RIGHT);
	fSlider->SetModificationMessage(new BMessage(VOLUME_CHANGED));
	fSlider->UseFillColor(true, &kVolumeGreen);
	fSlider->SetViewColor(B_TRANSPARENT_COLOR);
	AddChild(fSlider);

	float height = 20.0;
	fSlider->GetPreferredSize(NULL, &height);
	SetExplicitMaxSize(BSize(B_SIZE_UNSET, height + 16));
	fSlider->ResizeTo(Frame().Size());
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
