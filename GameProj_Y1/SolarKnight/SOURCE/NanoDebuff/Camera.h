/*!************************************************************************
\file       Camera.h
\project    Solar Knight
\author(s)  Chng Kai Rong, Leonard (k.chng) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/
#ifndef CAMERA_H
#define CAMERA_H

#include "pch.h"
#include "GameObject.h"

namespace Camera
{
	void CameraInit(f32& x, f32& y, const f32 width, const f32 height);
	void CameraPositionUpdate(f32 & x, f32& y, AEVec2& currentCamPos, GameObject::PlayerObject* player);

	void CameraLock(f32& camPosXRef, f32& camPosYRef,
		f32 const& halfWinWidthRef, f32 const& halfWinHeightRef,
		f32 const& levelXSizeRef, f32 const& levelYSizeRef);
}
#endif // !CAMERA_H
