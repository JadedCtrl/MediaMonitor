/*
 * Copyright 2022, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "PositionView.h"

#include <ControlLook.h>

#include "MediaPlayer.h"


const char* kPositionDesc = "PositionView is a replicant that shows the position of currently playing item.";

// The same color MediaPlayer uses, interface/SeekSlider
static const rgb_color kThumbRed = (rgb_color){ 255, 52, 52, 255 };

static const uint32 POSITION_CHANGED = 'vvch';


SeekSlider::SeekSlider(BRect frame, const char* name, BMessage* message, int32 minValue,
	int32 maxValue, orientation posture, uint32 resizingMode, uint32 flags)
	:
	BSlider(frame, name, NULL, message, minValue, maxValue, posture, B_TRIANGLE_THUMB, resizingMode,
		flags)
{
	rgb_color fillColor = tint_color(ui_color(B_PANEL_BACKGROUND_COLOR),
		B_DARKEN_3_TINT);
	UseFillColor(true, &fillColor);
}


void
SeekSlider::DrawThumb()
{
	if (!IsEnabled())
		return;

	BRect frame = ThumbFrame();
	be_control_look->DrawSliderTriangle(this, frame, frame, kThumbRed, 0,
		B_HORIZONTAL);
}


PositionView::PositionView(BRect frame)
	:
	ReplicantView(frame, "Position", B_FOLLOW_RIGHT, B_FOLLOW_LEFT_RIGHT,
		B_WILL_DRAW | B_DRAW_ON_CHILDREN | B_NAVIGABLE | B_FRAME_EVENTS)
{
	fDescription.SetTo(kPositionDesc);

	_InitInterface();
	Pulse();
}


PositionView::PositionView(BMessage* data)
	:
	ReplicantView(data)
{
	fDescription.SetTo(kPositionDesc);

	// For some reason, the BSlider gets archived with a wacko frame― better to just nuke it.
	fSlider = dynamic_cast<SeekSlider*>(FindView("seekSlider"));
	delete fSlider;

	_InitInterface();
	Pulse();
}


status_t
PositionView::Archive(BMessage* data, bool deep) const
{
	status_t status = ReplicantView::Archive(data, deep);

	data->AddString("class", "PositionView");
	data->AddString("add_on", APP_SIGNATURE);
	return status;	
}


PositionView*
PositionView::Instantiate(BMessage* data)
{
	if (!validate_instantiation(data, "PositionView"))
		return NULL;
	return new PositionView(data);
}


void
PositionView::AttachedToWindow()
{
	fSlider->SetTarget(this);
}


void
PositionView::MessageReceived(BMessage* msg)
{
	switch (msg->what) {
		case POSITION_CHANGED:
			fMediaPlayer->SetPosition(_SliderToPosition(fSlider->Position(),
				fMediaPlayer->Duration()));
			break;
		case B_MOUSE_WHEEL_CHANGED:
		{
			float scroll = 0.0f;
			if ((msg->FindFloat("be:wheel_delta_x", &scroll) == B_OK && scroll != 0.0f)
				|| (msg->FindFloat("be:wheel_delta_y", &scroll) == B_OK && scroll != 0.0f))
				fSlider->SetPosition(fSlider->Position() + scroll * -0.03);
			fMediaPlayer->SetPosition(_SliderToPosition(fSlider->Position(),
				fMediaPlayer->Duration()));
			break;
		}
		default:
			ReplicantView::MessageReceived(msg);
	}
}


void
PositionView::Pulse()
{
	int64 position = fMediaPlayer->Position();
	int64 duration = fMediaPlayer->Duration();
	if (duration > 0) {
		SetInactive(false);
		fSlider->SetPosition(_PositionToSlider(position, duration));
	} else
		SetInactive(true);
}


void
PositionView::SetInactive(bool inactive)
{
	fSlider->SetEnabled(!inactive);

	if (inactive && fTransparentInactivity && !fSlider->IsHidden())
		fSlider->Hide();
	else if ((!inactive || !fTransparentInactivity) && fSlider->IsHidden())
		fSlider->Show();

	ReplicantView::SetInactive(inactive);
}


void
PositionView::_InitInterface()
{
	SetViewColor(B_TRANSPARENT_COLOR);

	fSlider = new SeekSlider(BRect(), "seekSlider", NULL, 0, 1000, B_HORIZONTAL,
		B_FOLLOW_LEFT_RIGHT);
	fSlider->SetModificationMessage(new BMessage(POSITION_CHANGED));
	fSlider->SetViewColor(B_TRANSPARENT_COLOR);
	AddChild(fSlider);

	float height = 20.0;
	fSlider->GetPreferredSize(NULL, &height);
	SetExplicitMaxSize(BSize(B_SIZE_UNSET, height + 16));
	fSlider->ResizeTo(Frame().Size());
}


int64
PositionView::_SliderToPosition(float sliderPos, int64 duration)
{
	return duration * sliderPos;
}


float
PositionView::_PositionToSlider(int64 position, int64 duration)
{
	return (double)position / (double)duration;
}
