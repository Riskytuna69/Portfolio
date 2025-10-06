/*!************************************************************************
\file       Sound.cpp
\project    Solar Knight
\author(s) 	Chua Wen Shing Bryan (c.wenshingbryan) (100%)

\brief
    This is the sound manager cpp

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/
#include "AEEngine.h"
#include "Sound.h"

//
// Audio variables
//

AEAudio backGroundMusic;
AEAudioGroup bgm;

// Configure sound effects
AEAudio enemyHit, normalShot, chargedShot, timeDelay, charging ;
AEAudioGroup se;

void SoundInit(void)
{
    // Configure audio group
    backGroundMusic = AEAudioLoadMusic("Assets/Sounds/bgm.mp3");
    bgm = AEAudioCreateGroup(); // short for 'background music'


    // Configure sound effects
    enemyHit = AEAudioLoadSound("Assets/Sounds/Enemy_hit.mp3");
    se = AEAudioCreateGroup();   // short for 'sound effect'
    normalShot = AEAudioLoadSound("Assets/Sounds/Normal_shot.mp3");
    se = AEAudioCreateGroup();  
    chargedShot = AEAudioLoadSound("Assets/Sounds/Charged_shot.mp3");
    se = AEAudioCreateGroup();
    timeDelay = AEAudioLoadSound("Assets/Sounds/Time_delay.mp3");
    se = AEAudioCreateGroup();
    charging = AEAudioLoadSound("Assets/Sounds/Charging.mp3");
    se = AEAudioCreateGroup();
}


//AEAudioLoadMusic = BGM
//AEAudioLoadSound = SFX
//Use AEAudioLoadSound for multiple sound effects in game
void playSounds(SFX soundchoice, float volume)
{
    switch (soundchoice)
    {
        case SFX_MAINMENU:
        {
            AEAudioPlay(backGroundMusic, bgm, volume, 1.f, -1); // we can start playing here.
            break;
        }         
        case SFX_ENEMYHIT:
        {
            AEAudioPlay(enemyHit, se, volume, 1.f, 0); 
            break;
        } 
        case SFX_TIMESLOW:
        {
            AEAudioPlay(timeDelay, se, volume, 1.f, 0); 
            break;
        }
        case SFX_CHARGEDSHOT:
        {
            AEAudioPlay(chargedShot, se, volume, 1.f, 0); 
            break;
        }
        case SFX_NORMALSHOT:
        {
            AEAudioPlay(normalShot, se, volume, 1.f, 0); 
            break;
        }
        case SFX_CHARGING:
        {
            AEAudioPlay(charging, se, volume, 1.f, 0);
            break;
        }
        default:
            break;
    }
       
}

    //FOR UNLOADING SOUND 
    void unloadSounds(void)
    {
        // Unload audio resources
        AEAudioUnloadAudio(backGroundMusic);
        AEAudioUnloadAudio(enemyHit);
        AEAudioUnloadAudio(normalShot);
        AEAudioUnloadAudio(chargedShot);
        AEAudioUnloadAudio(timeDelay);
        AEAudioUnloadAudio(charging);

        AEAudioUnloadAudioGroup(bgm);
        AEAudioUnloadAudioGroup(se);
    }