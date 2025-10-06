/*!************************************************************************
\file       Button.cpp
\project    Solar Knight
\author(s)  Chng Kai Rong, Leonard (k.chng) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/
#include "Button.h"

namespace Button {
	bool checkButtonClick(AEVec2 mousePos, AEVec2 btnPos, AEVec2 btnSize)
	{
		f32 halfBtnSizeX = btnSize.x * 0.5f;
		f32 halfBtnSizeY = btnSize.y * 0.5f;
		if (mousePos.x < btnPos.x - halfBtnSizeX)
			return false;
		else if (mousePos.x > btnPos.x + halfBtnSizeX)
			return false;
		else if (mousePos.y < btnPos.y - halfBtnSizeY)
			return false;
		else if (mousePos.y > btnPos.y + halfBtnSizeY)
			return false;

		return true;
	}
}