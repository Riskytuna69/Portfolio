/* Start Header ************************************************************************/
/*!
* LEVEL 2
*/
/* End Header **************************************************************************/
#pragma once

namespace Level2 {
	void Init();		// Load assets (meshes)
	void Set();			// Set values (Eg. player position)
	void Update();		// Update things in level
	void Render();		// Render Graphics
	void Unset();		// Unset values?
	void Exit();		// Unload assets (meshes, pointers)
} // namespace Level2
