/*!************************************************************************
\file       Sound.h
\project    Solar Knight
\author(s) 	Chua Wen Shing Bryan (c.wenshingbryan) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/
#pragma once
enum SFX
{
	SFX_MAINMENU,
	SFX_ENEMYHIT,
	SFX_TIMESLOW,
	SFX_CHARGEDSHOT,
	SFX_NORMALSHOT,
	SFX_CHARGING
};


void SoundInit(void);
void unloadSounds(void);
void playSounds(SFX soundchoice, float volume);