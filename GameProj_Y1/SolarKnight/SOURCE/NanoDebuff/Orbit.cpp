/*!************************************************************************
\file       Orbit.cpp
\project    Solar Knight
\author(s)  Chng Kai Rong, Leonard (k.chng) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/
#include "Orbit.h"

namespace Orbit {
	void SunMoonInit(AEVec2& pos, f32& rotation, const f32& x, const f32& y)
	{
		pos.x = x;
		pos.y = y;
		rotation = 0.0f;
	}

	void SunMoonUpdate(AEVec2& pos, const AEVec2& camPos, const f32 offset)
	{
		pos.x = camPos.x;
		pos.y = camPos.y - offset;
	}

	f32 SunMoonRotationUpdate(DayNightCycle::DayNightState const& currState, f32 dt)
	{
		f32 rotAngle = 180.0f;						// total angles to rotate
		f32 degreeToRad = rotAngle * PI / 180.0f;	// convert to RAD
		f32 totalSunTime{ 0.0f };

		if (currState == DayNightCycle::STATE_DAY || currState == DayNightCycle::STATE_EVENING)
			totalSunTime = static_cast<f32>(DayNightCycle::MAX_DAY_TIME + DayNightCycle::MAX_EVENING_TIME - DayNightCycle::COLOR_TRANSITION_TIME / 2.0);
		else if (currState == DayNightCycle::STATE_NIGHT)
			totalSunTime = static_cast<f32>(DayNightCycle::MAX_NIGHT_TIME - DayNightCycle::COLOR_TRANSITION_TIME / 2.0);

		return degreeToRad / totalSunTime * dt;
	}
}
