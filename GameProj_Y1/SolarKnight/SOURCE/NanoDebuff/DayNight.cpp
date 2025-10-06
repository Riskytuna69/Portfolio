/*!************************************************************************
\file       DayNight.cpp
\project    Solar Knight
\author(s)  Chng Kai Rong, Leonard (k.chng) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/
#include "DayNight.h"
#include <AEEngine.h>

namespace DayNightCycle
{
	const f32 MAX_DAY_TIME = 70.0f;
	const f32 MAX_EVENING_TIME = 20.0f;
	const f32 MAX_NIGHT_TIME = 30.0f;
	const f32 COLOR_TRANSITION_START = 3.0f;
	const f32 COLOR_TRANSITION_TIME = 6.0f;
	f32 dayTimer = { 0.0f };
	f32 eveningTimer = { 0.0f };
	f32 nightTimer = { 0.0f };
	DayNightState eDNState = STATE_NULL;

	void Initialize(DayNightState& currState, DayNightState& prevState,
		f32& dayTime, f32& eveningTime, f32& nightTime, 
		Colors::Color& skyColor, bool& bSkyColorTransiting, f32& skyTranisitionTime)
	{
		if (DayNightCycle::eDNState == DayNightCycle::STATE_NULL)
		{
			DayNightCycle::Reset();
			currState = DayNightCycle::eDNState;
			dayTime = DayNightCycle::MAX_DAY_TIME;
			eveningTime = DayNightCycle::MAX_EVENING_TIME;
			nightTime = DayNightCycle::MAX_NIGHT_TIME;
			skyColor = Colors::cyan;
		}
		else
		{
			DayNightCycle::Load(&currState, &dayTime, &eveningTime, &nightTime);
			switch (currState)
			{
			case DayNightCycle::STATE_DAY:
				skyColor = Colors::cyan;
				break;
			case DayNightCycle::STATE_EVENING:
				skyColor = Colors::orange;
				break;
			case DayNightCycle::STATE_NIGHT:
				skyColor = Colors::black;
				break;
			case DayNightCycle::STATE_NULL:
				break;
			default:
				break;
			}
		}
		prevState = DayNightCycle::STATE_NULL;
		bSkyColorTransiting = false;
		skyTranisitionTime = DayNightCycle::COLOR_TRANSITION_TIME;
	}

	void Reset()
	{
		eDNState = STATE_DAY;
		dayTimer = MAX_DAY_TIME;
		eveningTimer = MAX_EVENING_TIME;
		nightTimer = MAX_NIGHT_TIME;
	}

	void Save(DayNightState state, f32 dayT, f32 eveningT, f32 nightT)
	{
		eDNState = state;
		dayTimer = dayT;
		eveningTimer = eveningT;
		nightTimer = nightT;
	}

	void Load(DayNightState* state, f32* dayT, f32* eveningT, f32* nightT)
	{
		*state = eDNState;
		*dayT = dayTimer;
		*eveningT = eveningTimer;
		*nightT = nightTimer;
	}

	void DayNightUpdate(DayNightState& currState, DayNightState& prevState,
		f32& dayTime, f32& eveningTime, f32& nightTime, bool& bSkyTransit, f32 dt)
	{
		if (currState == STATE_DAY)
		{
			dayTime -= dt;
			if (dayTime <= COLOR_TRANSITION_START)
			{
				prevState = currState;
				bSkyTransit = !bSkyTransit;
			}
		}
		else if (currState == STATE_EVENING)
		{
			eveningTime -= dt;
			if (eveningTime <= COLOR_TRANSITION_START)
			{
				prevState = currState;
				bSkyTransit = !bSkyTransit;
			}

		}
		else if (currState == STATE_NIGHT)
		{
			nightTime -= dt;
			if (nightTime <= COLOR_TRANSITION_START)
			{
				prevState = currState;
				bSkyTransit = !bSkyTransit;
			}

		}
	}

	void DayNightStateChange(DayNightState& currState, f32& time, f32& sunRot)
	{
		switch (currState)
		{
		case STATE_DAY:
		{
			currState = STATE_EVENING;
			time = MAX_DAY_TIME;
			break;
		}
		case STATE_EVENING:
		{
			currState = STATE_NIGHT;
			time = MAX_EVENING_TIME;
			sunRot = 0.0f;
			break;
		}
		case STATE_NIGHT:
		{
			currState = STATE_DAY;
			time = MAX_NIGHT_TIME;
			sunRot = 0.0f;
			break;
		}
		default:
			break;
		}
	}

	void SkyColorTransition(bool& bSkyColorTransiting, f32& skyTranisitionTime, DayNightState& prevState, Colors::Color& skyColor, f32 gameDT)
	{
		if (bSkyColorTransiting)
		{
			skyTranisitionTime -= gameDT;
			if (skyTranisitionTime >= 0.0)
			{
				float colorTimer = static_cast<float>(DayNightCycle::COLOR_TRANSITION_TIME);
				switch (prevState)
				{
				case DayNightCycle::STATE_DAY:
					skyColor = Colors::ColorTransition(skyColor, Colors::orange, colorTimer, gameDT);
					break;
				case DayNightCycle::STATE_EVENING:
					skyColor = Colors::ColorTransition(skyColor, Colors::black, colorTimer, gameDT);
					break;
				case DayNightCycle::STATE_NIGHT:
					skyColor = Colors::ColorTransition(skyColor, Colors::cyan, colorTimer, gameDT);
					break;
				case DayNightCycle::STATE_NULL:
					break;
				default:
					break;
				}
			}
			else
			{
				bSkyColorTransiting = !bSkyColorTransiting;
				skyTranisitionTime = DayNightCycle::COLOR_TRANSITION_TIME;
			}
		}
	}
}
