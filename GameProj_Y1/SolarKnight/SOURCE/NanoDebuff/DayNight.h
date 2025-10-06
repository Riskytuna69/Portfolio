/*!************************************************************************
\file       DayNight.h
\project    Solar Knight
\author(s)  Chng Kai Rong, Leonard (k.chng) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/
#ifndef DAYNIGHT_H
#define DAYNIGHT_H

#include "AETypes.h"
#include "Color.h"
namespace DayNightCycle
{
	typedef enum DayNightState
	{
		STATE_DAY = 0,
		STATE_EVENING,
		STATE_NIGHT,
		STATE_NULL
	}DayNightState;

	extern const f32 MAX_DAY_TIME;
	extern const f32 MAX_EVENING_TIME;
	extern const f32 MAX_NIGHT_TIME;
	extern const f32 COLOR_TRANSITION_START;
	extern const f32 COLOR_TRANSITION_TIME;
	extern f32 dayTimer, eveningTimer, nightTimer;
	extern DayNightState eDNState;

	void Initialize(DayNightState& currState, DayNightState& prevState,
		f32& dayTime, f32& eveningTime, f32& nightTime,
		Colors::Color& skyColor, bool& bSkyColorTransiting, f32& skyTranisitionTime);

	void Reset();

	void Save(DayNightState state, f32 dayT, f32 eveningT, f32 nightT);

	void Load(DayNightState* state, f32* dayT, f32* eveningT, f32* nightT);

	void DayNightUpdate(DayNightState& currState, DayNightState& prevState,
		f32& dayTime, f32& eveningTime, f32& nightTime, bool& bSkyTransit, f32 dt);

	void DayNightStateChange(DayNightState& currState, f32& time, f32& sunRot);

	void SkyColorTransition(bool &bSkyColorTransiting, f32& skyTranisitionTime, DayNightState& prevState, Colors::Color& skyColor, f32 gameDT);
}
#endif