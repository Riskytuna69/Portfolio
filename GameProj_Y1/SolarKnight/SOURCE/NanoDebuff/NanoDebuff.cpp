/*!************************************************************************
\file       NanoDebuff.cpp
\project    Solar Knight
\author(s)  Lau Jia Win (jiawin.lau) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/

#include "pch.h"

#include "GameStateManager.h"
#include "System.h"

#include <crtdbg.h> // To check for memory leaks

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    //Systems initialize
    System::Initialize(hInstance, nCmdShow);

    //GSM initialize
    GSM::Initialize(GS_SPLASHSCREEN);

    while (GSM::stateCurrent != GS_QUIT)
    {
        // State has been changed when (next != current)
        // Find out which state to switch to
        if (GSM::stateCurrent != GS_RESTART)
        {
            // Change level
            GSM::Update();  // Update game state manager
            GSM::pInit();   // Init current game state
        }
        else
        {
            // Restarting current level
            GSM::stateNext = GSM::stateCurrent = GSM::statePrev;
        }

        GSM::pSet();        // Set current game state

        // Game loop
        // =================================================
        // Keep looping until next state is different 
        while (GSM::stateNext == GSM::stateCurrent)
        {
            // Informing the system about the loop's start
            AESysFrameStart();

            GSM::pUpdate();     // Update current game state
            GSM::pRender();     // Render current game state

            // Informing the system about the loop's end
            AESysFrameEnd();

            // check if forcing the application to quit
            if (AESysDoesWindowExist() == false) {
                GSM::stateNext = GS_QUIT;
            }

            // Manage delta time...
            //gameDeltaTime = AEFrameRateControllerGetFrameTime();

            // Cap the game loop to 1/60 (60 frames per second???)
            // ...
            
            // Reset game total time in case overflow
            // ...
        }
        // =================================================

        // Exited Game loop (either restart or change level)
        GSM::pUnset();           // Cleans data/prepare for restart

        // Check if going to new level or restart current level
        if (GSM::stateNext != GS_RESTART)
        {
            // Going new level
            GSM::pExit();        // Unload current game state
        }

        GSM::statePrev = GSM::stateCurrent;     // Set previous state as currently
        GSM::stateCurrent = GSM::stateNext;     // Set current state to next
    }

    //Systems exit (terminate)
    System::Exit();    
}