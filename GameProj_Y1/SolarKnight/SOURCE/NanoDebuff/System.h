/*!************************************************************************
\file       System.h
\project    Solar Knight
\author(s)  Lau Jia Win (jiawin.lau) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/

#pragma once

#ifndef SYSTEM_H
#define SYSTEM_H

namespace System {
	/*!***********************************************************************
	\brief
		Initalize the base system for the game.

	\return
		Void.
	*************************************************************************/
	void Initialize(_In_ HINSTANCE hInstance, _In_ int nCmdShow);

	/*!***********************************************************************
	\brief
		Unload resources and allocated memory used by the system.

	\return
		Void.
	*************************************************************************/
	void Exit();
} // namespace System
#endif