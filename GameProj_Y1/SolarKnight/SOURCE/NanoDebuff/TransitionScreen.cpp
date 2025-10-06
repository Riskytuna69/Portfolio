/*!************************************************************************
\file       TransitionScreen.cpp
\project    Solar Knight
\author(s)  Lau Jia Win (jiawin.lau) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/

#include "TransitionScreen.h"

namespace GR
{
	TransitionScreen::TransitionScreen(float fadeWholeDuration, float windowWidth, void(*doneFadeInCallThis)(void))
		: callUponFadeInComplete{ doneFadeInCallThis },
		fadeIn{ true },
		fadeTimer{ fadeWholeDuration },
		FADEMAX{ fadeWholeDuration },
		positionX{ -windowWidth },
		RATEOFCHANGE{ fadeWholeDuration ? windowWidth / fadeWholeDuration : windowWidth / 1.f },
		WINDOWWIDTH{ windowWidth }
	{
	}

	bool TransitionScreen::IsFadingInOut(float const& dt)
	{
		if (fadeIn)
		{
			// FADE IN
			fadeTimer -= dt;
			positionX += RATEOFCHANGE * dt;

			if (fadeTimer < 0)
			{
				// FADE IN COMPLETE
				fadeIn = false;
				callUponFadeInComplete();
				fadeTimer = FADEMAX;
				positionX = 0;
			}
		}
		else
		{
			// FADE OUT
			fadeTimer -= dt;
			positionX += RATEOFCHANGE * dt;

			if (fadeTimer < 0)
			{
				fadeIn = true;
				fadeTimer = FADEMAX;
				positionX = -WINDOWWIDTH;

				// COMPLETE CYCLE
				return false;
			}
		}

		return true;
	}

	float const& TransitionScreen::PositionX()
	{
		return positionX;
	}
}