/*!************************************************************************
\file       Level1.h
\project    Solar Knight
\author(s)  Lau Jia Win (jiawin.lau) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/
#ifndef LEVEL1_H
#define LEVEL1_H
#include "GameObject.h"

namespace Level1 {
	void Init();		// Load assets (meshes)
	void Set();			// Set values (Eg. player position)
	void Update();		// Update things in level
	void Render();		// Render Graphics
	void Unset();		// Unset values?
	void Exit();		// Unload assets (meshes, pointers)

	void PrepareSwitchRooms(GameObject::GameObject const* doorGOThatCollided);
	void SwitchRooms(bool firstTime = false);
	void SwitchRoomProxy();

	void PlayerDied();
} // namespace Level1
#endif