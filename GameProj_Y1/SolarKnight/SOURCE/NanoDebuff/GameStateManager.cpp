/*!************************************************************************
\file       GameStateManager.cpp
\project    Solar Knight
\author(s)  Lau Jia Win (jiawin.lau) (70%)
\author(s)  Chng Kai Rong, Leonard (k.chng) (30%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/
#include "pch.h"
#include <iostream>

#include "GameStateManager.h"
#include "Level1.h"
#include "Splashscreen.h"
#include "MainMenu.h"
#include "Credits.h"

namespace GSM {
	int stateCurrent = { 0 }, statePrev = { 0 }, stateNext = { 0 };

	GSMPointer pInit = { nullptr }, pSet = { nullptr },
		pUpdate = { nullptr }, pRender = { nullptr },
		pUnset = { nullptr }, pExit = { nullptr };

	void Initialize(int startingState)
	{
		stateCurrent = statePrev = stateNext = startingState;
	}

	void Update()
	{
		// Change pointers to point to appropriate functions
		switch (stateCurrent)
		{
		case GS_LEVEL1:
			pInit = Level1::Init;
			pSet = Level1::Set;
			pUpdate = Level1::Update;
			pRender = Level1::Render;
			pUnset = Level1::Unset;
			pExit = Level1::Exit;
			break;
		case GS_SPLASHSCREEN:
			pInit = Splashscreen::Init;
			pSet = Splashscreen::Set;
			pUpdate = Splashscreen::Update;
			pRender = Splashscreen::Render;
			pUnset = Splashscreen::Unset;
			pExit = Splashscreen::Exit;
			break;
		case GS_MAINMENU:
			pInit = MainMenu::Init;
			pSet = MainMenu::Set;
			pUpdate = MainMenu::Update;
			pRender = MainMenu::Render;
			pUnset = MainMenu::Unset;
			pExit = MainMenu::Exit;
			break;
		case GS_CREDITS:
			pInit = Credits::Init;
			pSet = Credits::Set;
			pUpdate = Credits::Update;
			pRender = Credits::Render;
			pUnset = Credits::Unset;
			pExit = Credits::Exit;
			break;
		case GS_RESTART:
			break;
		case GS_QUIT:
			break;
		default:
			break;
		}

	}
} // namespace GSM
