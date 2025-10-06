/*!************************************************************************
\file       Color.cpp
\project    Solar Knight
\author(s)  Chng Kai Rong, Leonard (k.chng) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/
#include "Color.h"

namespace Colors
{
	Color ColorTransition(const Color& start, const Color& end, float time, f32 gameDT)
	{
		float r = start.r + ((end.r - start.r) / time) * time * gameDT;
		float g = start.g + ((end.g - start.g) / time) * time * gameDT;
		float b = start.b + ((end.b - start.b) / time) * time * gameDT;
		return Color{ r, g, b };
	}

}
