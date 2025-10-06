/******************************************************************************/
/*!
\file   PauseSystem.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   04/02/2025

\author Chan Kuan Fu Ryan (100%)
\par    email: c.kuanfuryan\@digipen.edu
\par    DigiPen login: c.kuanfuryan

\brief
    PauseSystem is an ECS system that does controls logic for pause screen
    while the default scene is loaded (game scene).

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#include "PauseSystem.h"
#include "GameOver.h"
#include "SceneManagement.h"
#include "AudioManager.h"
#include "game.h"
#include "GameSettings.h"
#include "GameManager.h"
#include "TweenManager.h"

bool PauseSystem::PreRun()
{
    // If ESC is pressed
    if (Input::GetKeyPressed(KEY::ESC))
    {
        // If not in game state, return
        if (ST<Game>::Get()->GetState() != GAMESTATE::IN_GAME) return false;

        // If GameOver screen is showing, return
        auto gameOverCompIter{ ecs::GetCompsBegin<GameOverComponent>() }; // Not a cheap operation, so we put it in the keypress check
        bool isGameOver{ gameOverCompIter != ecs::GetCompsEnd<GameOverComponent>() && gameOverCompIter->GetPlayState() != PLAYSTATE::PLAY };
        if (isGameOver) return false;
        
        // If the game is already paused, unpause it
        if (ST<GameManager>::Get()->GetPaused())
        {
            Scene* pause = ST<SceneManager>::Get()->GetSceneWithName("PauseScene");
            if (!pause || !ST<SceneManager>::Get()->CheckCanUnloadScene(pause->GetIndex()))
            {
                return false;
            }
            ST<SceneManager>::Get()->UnloadScene(pause->GetIndex());
        }
        // Additional check to see if game scene is loaded, before pausing
        else if (
            ST<SceneManager>::Get()->CheckIsSceneLoaded("NEW_LEVEL") ||
            ST<SceneManager>::Get()->CheckIsSceneLoaded("TutorialScene"))
        {
            ST<SceneManager>::Get()->LoadScene(ST<Filepaths>::Get()->scenesSave + "/PauseScene.scene");
        }
    }

    // If the pause scene is loaded and the game manager is not paused, execute pause functions
    if (ST<SceneManager>::Get()->CheckIsSceneLoaded("PauseScene") && !ST<GameManager>::Get()->GetPaused())
    {
        ST<GameManager>::Get()->SetPaused(true);
        // ST<AudioManager>::Get()->PauseAllSounds();
        ST<AudioManager>::Get()->SetGroupPitch(0.6f, "BGM");
        ST<AudioManager>::Get()->SetGroupVolume(2.0f, "BGM");
        GameTime::SetTimeScale(0.0f);
    }
    // If the pause scene is not loaded and the game manager is already paused, execute unpause functions
    else if (!ST<SceneManager>::Get()->CheckIsSceneLoaded("PauseScene") && ST<GameManager>::Get()->GetPaused())
    {
        ST<GameManager>::Get()->SetPaused(false);
        // ST<AudioManager>::Get()->ResumeAllSounds();
        ST<AudioManager>::Get()->SetGroupPitch(1.0f, "BGM");
        ST<AudioManager>::Get()->SetGroupVolume(1.0f, "BGM");
        GameTime::SetTimeScale(1.0f);
    }
    
	return true;
}
