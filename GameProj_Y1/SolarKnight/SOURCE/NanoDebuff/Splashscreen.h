/*!************************************************************************
\file       Splashscreen.h
\project    Solar Knight
\author(s)  Chng Kai Rong, Leonard (k.chng) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/
#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H
namespace Splashscreen {
	void Init();		// Load assets (meshes)
	void Set();			// Set values (Eg. player position)
	void Update();		// Update things in level
	void Render();		// Render Graphics
	void Unset();		// Unset values?
	void Exit();		// Unload assets (meshes, pointers)
} // namespace Splashscreen
#endif