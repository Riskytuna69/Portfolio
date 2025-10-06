/*!************************************************************************
\file       Button.h
\project    Solar Knight
\author(s)  Chng Kai Rong, Leonard (k.chng) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/
#ifndef BUTTON_H
#define BUTTON_H
#include "pch.h"
namespace Button {
	typedef struct Button {
		AEVec2 pos;
		AEVec2 size;
	}Button;

	bool checkButtonClick(AEVec2 mousePos, AEVec2 btnPos, AEVec2 btnSize);
}
#endif
