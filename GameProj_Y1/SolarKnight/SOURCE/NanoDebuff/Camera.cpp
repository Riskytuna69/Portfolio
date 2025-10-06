/*!************************************************************************
\file       Camera.cpp
\project    Solar Knight
\author(s)  Chng Kai Rong, Leonard (k.chng) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/
#include "Camera.h"

namespace Camera
{
	void CameraInit(f32&x, f32& y, const f32 width, const f32 height)
	{
		x = width;
		y = height;
	}

	void CameraPositionUpdate(f32& x, f32& y, AEVec2& currentCamPos,
		GameObject::PlayerObject* player)
	{
		// update camera position with lerp
		AEVec2 finalCamPos{};
		currentCamPos.x = x;
		currentCamPos.y = y;
		AEVec2Lerp(&finalCamPos, &currentCamPos, &player->go->position, 0.1f);
		x = finalCamPos.x;
		y = finalCamPos.y;
		AEGfxSetCamPosition(currentCamPos.x, currentCamPos.y);
	}
	
	void CameraLock(f32& camPosXRef, f32& camPosYRef,
		f32 const& halfWinWidthRef, f32 const& halfWinHeightRef,
		f32 const& levelXSizeRef, f32 const& levelYSizeRef)
	{
		{
			// Clamp Left
			if (camPosXRef < halfWinWidthRef)
			{
				camPosXRef = halfWinWidthRef;
			}
			// Clamp Right
			if (camPosXRef > levelXSizeRef * 0.5f - halfWinWidthRef)
			{
				camPosXRef = levelXSizeRef * 0.5f - halfWinWidthRef;
			}
			// Clamp Bot
			if (camPosYRef < halfWinHeightRef)
			{
				camPosYRef = halfWinHeightRef;
			}
			// Clamp Top
			if (camPosYRef > levelYSizeRef * 0.5f - halfWinHeightRef)
			{
				camPosYRef = levelYSizeRef * 0.5f - halfWinHeightRef;
			}
		}
	}
}