/*!************************************************************************
\file       GameStateManager.h
\project    Solar Knight
\author(s)  Lau Jia Win (jiawin.lau) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/

#pragma once

namespace GSM {
	typedef void(*GSMPointer)(void);

	extern int stateCurrent, statePrev, stateNext;

	extern GSMPointer pInit, pSet, pUpdate, pRender, pUnset, pExit;

	void Initialize(int startingState);
	void Update();
} // namespace GSM
