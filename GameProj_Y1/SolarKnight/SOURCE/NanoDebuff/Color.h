/*!************************************************************************
\file       Color.h
\project    Solar Knight
\author(s)  Chng Kai Rong, Leonard (k.chng) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/
#ifndef COLOR_H
#define COLOR_H

#include "pch.h"

namespace Colors
{
	typedef struct Color
	{
		float r;
		float g;
		float b;
		float a;
	}Color;

	constexpr Color cyan{ 0.529f, 0.808f, 0.92f };
	constexpr Color black{ 0.001f, 0.001f, 0.001f };
	constexpr Color orange{ 0.992f, 0.772f, 0.075f };

	Color ColorTransition(const Color& start, const Color& end, float time, f32 gameDT);

}
#endif