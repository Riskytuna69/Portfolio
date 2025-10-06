/*!************************************************************************
\file       GameVictory.cpp
\project    Solar Knight
\author(s)  Lau Jia Win (jiawin.lau) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/

#include "GameVictory.h"

namespace GameVictory {

	void Initialize()
	{
		timer = DISPLAYTIME;
	}

	// Returns false
	bool DisplayVictory(float const& dt)
	{
		timer -= dt;
		if (timer < 0)
		{
			return false;
		}

		// Call render

		return true;
	}
}