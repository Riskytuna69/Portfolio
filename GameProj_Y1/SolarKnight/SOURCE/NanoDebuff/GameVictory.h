/*!************************************************************************
\file       GameVictory.h
\project    Solar Knight
\author(s)  Lau Jia Win (jiawin.lau) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/

#pragma once

namespace GameVictory {
	
	constexpr float DISPLAYTIME{ 2.f };
	
	namespace {
		float timer{};
	}

	void Initialize();

	bool DisplayVictory(float const& dt);

}