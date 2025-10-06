/*!************************************************************************
\file       Credits.h
\project    Solar Knight
\author(s)  Chng Kai Rong, Leonard (k.chng) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/
#ifndef CREDITS_H
#define CREDITS_H
namespace Credits {
	void Init();		// Load assets (meshes)
	void Set();			// Set values (Eg. player position)
	void Update();		// Update things in level
	void Render();		// Render Graphics
	void Unset();		// Unset values?
	void Exit();		// Unload assets (meshes, pointers)
} // namespace Credits
#endif