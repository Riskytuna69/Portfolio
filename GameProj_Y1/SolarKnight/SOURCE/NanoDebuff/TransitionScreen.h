/*!************************************************************************
\file       TransitionScreen.h
\project    Solar Knight
\author(s)  Lau Jia Win (jiawin.lau) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/

#pragma once

namespace GR
{
	class TransitionScreen
	{
	public:
		TransitionScreen(float fadeWholeDuration,
			float windowWidth,
			void (*doneFadeInCallThis)(void));

		// Prevents other constructors
		TransitionScreen() = delete;
		TransitionScreen(TransitionScreen const&) = delete;
		TransitionScreen& operator=(TransitionScreen const&) = delete;

		// Returns true if still fading in/out
		// Returns false if complete one cycle
		bool IsFadingInOut(float const& dt);

		// Returns current X position of splashscreen
		float const& PositionX();

	private:
		// Call when fade in complete
		void (*callUponFadeInComplete)(void);

		// Timings
		bool fadeIn;
		float fadeTimer;
		float const FADEMAX;

		// Variables to modify
		float positionX;

		// Other consts
		float const RATEOFCHANGE; // windowWidth / total time
		float const WINDOWWIDTH;
	};
}