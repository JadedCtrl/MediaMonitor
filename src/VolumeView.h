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
	        void        _InitInterface();
	        float       _VolumeToPosition(float volume);
	        float       _PositionToVolume(float position);

	BSlider* fSlider;
};

#endif // VOLUMEVIEW_H
