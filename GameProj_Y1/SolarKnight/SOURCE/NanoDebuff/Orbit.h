/*!************************************************************************
\file       Orbit.h
\project    Solar Knight
\author(s)  Chng Kai Rong, Leonard (k.chng) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/
#ifndef ORBIT_H
#define ORBIT_H

#include "pch.h"
#include "DayNight.h"

namespace Orbit{
	void SunMoonInit(AEVec2& pos, f32& rotation, const f32& x = 0.0f, const f32& y = 0.0f);

	void SunMoonUpdate(AEVec2& pos, const AEVec2& camPos, const f32 offset);

	f32 SunMoonRotationUpdate(DayNightCycle::DayNightState const& currState, f32 dt);
}


#endif // ORBIT_H
