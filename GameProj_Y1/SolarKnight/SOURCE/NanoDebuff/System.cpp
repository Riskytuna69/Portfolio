/*!************************************************************************
\file       System.cpp
\project    Solar Knight
\author(s)  Lau Jia Win (jiawin.lau) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/

#include "pch.h"

#include "System.h"

namespace System {
	
	void Initialize(_In_ HINSTANCE hInstance, _In_ int nCmdShow)
	{
		s32 createConsole = 0;
		s32 windowWidth = 1600;
		s32 windowHeight = 900;
		int maxFrame = 60;
		// Using custom window procedure
		AESysInit(hInstance, nCmdShow, windowWidth, windowHeight, createConsole, maxFrame, true, NULL);

		// Changing the window title
		AESysSetWindowTitle("Solar Knight");

		// reset the system modules
		AESysReset();
	}

	void Exit()
	{
		// free the system
		AESysExit();
	}
} // namespace System
