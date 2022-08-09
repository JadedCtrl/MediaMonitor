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
#ifndef POSITIONVIEW_H
#define POSITIONVIEW_H

#include <Slider.h>

#include "ReplicantView.h"


class SeekSlider : public BSlider {
public:
	             SeekSlider(BRect frame, const char* name, BMessage* message, int32 minValue,
		                    int32 maxValue, orientation posture,
		                    uint32 resizingMode = B_FOLLOW_LEFT_TOP,
		                    uint32 flags = B_NAVIGABLE | B_WILL_DRAW | B_FRAME_EVENTS);

	virtual void DrawThumb();
};


class PositionView : public ReplicantView {
public:
	                    PositionView(BRect frame);
	                    PositionView(BMessage* archive);

	virtual status_t    Archive(BMessage* data, bool deep = true) const;
	static PositionView*  Instantiate(BMessage* data);

	        void        AttachedToWindow();

	virtual void        MessageReceived(BMessage* msg);
	virtual void        Pulse();

	virtual void        SetInactive(bool inactive);

private:
	        void        _InitInterface();
	        int64       _SliderToPosition(float volume, int64 duration);
	        float       _PositionToSlider(int64 position, int64 duration);

	SeekSlider* fSlider;
};

#endif // POSITIONVIEW_H
