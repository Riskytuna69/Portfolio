/*!************************************************************************
\file       MainMenu.h
\project    Solar Knight
\author(s)  Chng Kai Rong, Leonard (k.chng) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/
#ifndef MAINMENU_H
#define MAINMENU_H
namespace MainMenu {
	void Init();		// Load assets (meshes)
	void Set();			// Set values (Eg. player position)
	void Update();		// Update things in level
	void Render();		// Render Graphics
	void Unset();		// Unset values?
	void Exit();		// Unload assets (meshes, pointers)
} // namespace MainMenu
#endif