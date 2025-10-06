/*!************************************************************************
\file       MapLevel.fwd.h
\project    Solar Knight
\author(s)  Lau Jia Win (jiawin.lau) (100%)

\brief
	This file forward declares MapLevel.
	(Declares, but does not include the whole definition into the 
	 file that included this foward declaration).
	(This prevents cyclic references,
	 foo.h includes boo.h but boo.h also includes foo.h, ...ENDLESS!).

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/

#pragma once
namespace Map2 {
	class MapLevel;
}