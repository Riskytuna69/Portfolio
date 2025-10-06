/******************************************************************************/
/*!
\file   Parallax.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   05/02/2025

\author Chua Wen Shing Bryan (100%)
\par    email: c.wenshingbryan\@digipen.edu
\par    DigiPen login: c.wenshingbryan

\brief

    Function definations for the parallax effects in game


All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/

#include "Parallax.h"
#include "CameraComponent.h"
#include "GameTime.h"
ParallaxComponent::ParallaxComponent() :  parallaxMode(Follow), gameState(Menu), currIntensity(FrontLayer),
                                             prevTargetPos({0,0}), menuYPos(0), setAnchor(false)
#ifdef IMGUI_ENABLED
    , REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw)
#endif 
{

}

void ParallaxComponent::SetPrevTargetPos(Vector2 val){ prevTargetPos = val; }
void ParallaxComponent::SetMenuYPos(float val) { menuYPos = val; }
float ParallaxComponent::GetMenuYPos() { return menuYPos; }
Vector2 ParallaxComponent::GetPrevTargetPos() { return prevTargetPos; }

#ifdef IMGUI_ENABLED
void ParallaxComponent::EditorDraw(ParallaxComponent& comp)
{
    const char* GamMode[] = { "Menu", "InGame" };
    const char* ParaMode[] = { "Follow (Menu Only)", "Opposite (Menu Only)" }; 
    const char* IntensityMode[] = { "Front", "Middle", "Back"};
    static int item_current = 0, state_current = 0, intensity_current = 0;
    
    //Assign orginal settings on this comp to the values first
    state_current = comp.gameState;
    item_current = comp.parallaxMode;
    intensity_current = comp.currIntensity;

    ImGui::Combo("Game State", &state_current, GamMode, IM_ARRAYSIZE(GamMode));
    comp.gameState = state_current;

    ImGui::Combo("Parallax Mode", &item_current, ParaMode, IM_ARRAYSIZE(ParaMode));
    comp.parallaxMode = item_current;

    ImGui::Combo("Intensity", &intensity_current, IntensityMode, IM_ARRAYSIZE(IntensityMode));
    comp.currIntensity = intensity_current;
}
#endif

ParallaxSystem::ParallaxSystem()
    : System_Internal(&ParallaxSystem::UpdateParallaxComp)
{
}


void ParallaxSystem::UpdateParallaxComp(ParallaxComponent& comp)
{   
    if (comp.gameState == Menu)
    {
        //Mouse did not move
        if (static_cast<int>(comp.GetPrevTargetPos().x) == static_cast<int>(Input::GetMousePosWorld().x)) return;

        float displacementX = Input::GetMousePosWorld().x - comp.GetPrevTargetPos().x;
        float displacementY = Input::GetMousePosWorld().y - comp.GetPrevTargetPos().y;
        float currY = ecs::GetEntity(&comp)->GetTransform().GetWorldPosition().y ;
        float intensity = (comp.currIntensity == FrontLayer) ? comp.intensityFrontLayer :
                          (comp.currIntensity == MiddleLayer) ? comp.intensityMidLayer :
                          (comp.currIntensity == BackLayer) ? comp.intensityBackLayer : 0;

        float prevMousePosX = comp.GetPrevTargetPos().x;
        float prevMousePosY = comp.GetPrevTargetPos().y;
        int dirX = 1, dirY = 1;

        //Sets the original pos of the background object, default constructor will set it to 0 at first
        if (comp.GetMenuYPos() == 0) comp.SetMenuYPos(currY);

        //Im putting '=' in the '<=' cos im paranoid 
        if (displacementX <= 0) //Mouse moved left
        {
            //Determine which side of the screen the mouse was in the previous frame
            //We need this to set the position in the correct sign 
            dirX = (prevMousePosX < 0) ? 1 : -1;
           
            if (comp.parallaxMode == Follow) dirX *= -1;
            else if (comp.parallaxMode == Opposite) dirX *= 1;
        }
        else if (displacementX > 0) //Mouse moved right
        {
            dirX = (prevMousePosX > 0) ? 1 : -1;

            if (comp.parallaxMode == Follow) dirX *= 1;
            else if (comp.parallaxMode == Opposite) dirX *= -1;   
        }

        if (displacementY <= 0) //mouse moved down
        {
            dirY = (prevMousePosY < 0) ? 1 : -1;

            if (comp.parallaxMode == Follow) dirY *= -1;
            else if (comp.parallaxMode == Opposite) dirY *= 1;
        }
        else if (displacementY > 0) //mouse moved up
        {
            dirY = (prevMousePosY > 0) ? 1 : -1;

            if (comp.parallaxMode == Follow) dirY *= 1;
            else if (comp.parallaxMode == Opposite) dirY *= -1;
        }

        // Disable y movement
        /*
        ecs::GetEntity(&comp)->GetTransform().SetWorldPosition(
            Vector2(std::fabs(Input::GetMousePosWorld().x) * dirX * intensity, 
            ((std::fabs(Input::GetMousePosWorld().y) * dirY * (intensity/2.f)) + comp.GetMenuYPos() ))
        );
        */

        // Hi, it's Ryan Chan here. Just disabling the Y for now.
        ecs::EntityHandle compEntity = ecs::GetEntity(&comp);
        compEntity->GetTransform().SetWorldPosition(
            Vector2(std::fabs(Input::GetMousePosWorld().x) * dirX * intensity,
            compEntity->GetTransform().GetWorldPosition().y)
        );
        // My code ends here.

        comp.SetPrevTargetPos(Input::GetMousePosWorld());
    }
    else if (comp.gameState == InGame)
    {
        auto cameraCompIter{ ecs::GetCompsBegin<CameraComponent>() };
        auto playercompiter{ ecs::GetCompsBegin<PlayerComponent>() };

        //break out if no player is found
        if (cameraCompIter == ecs::GetCompsEnd<CameraComponent>()) return;

        //Player did not move
        if (static_cast<int>(comp.GetPrevTargetPos().x) == static_cast<int>(cameraCompIter.GetEntity()->GetTransform().GetWorldPosition().x)) return;
      
        float currY = ecs::GetEntity(&comp)->GetTransform().GetWorldPosition().y;
        float intensity = (comp.currIntensity == FrontLayer) ? comp.intensityFrontLayer :
                          (comp.currIntensity == MiddleLayer) ? comp.intensityMidLayer :
                          (comp.currIntensity == BackLayer) ? comp.intensityBackLayer : 0.f;
  
        if (!comp.setAnchor)
        {
            comp.SetPrevTargetPos({ ecs::GetEntity(&comp)->GetTransform().GetWorldPosition().x + (ecs::GetEntity(&comp)->GetTransform().GetWorldScale().x * 2.5f),
                                    ecs::GetEntity(&comp)->GetTransform().GetWorldPosition().y });
            comp.setAnchor = true;
        }
        float distDiff = std::abs(cameraCompIter.GetEntity()->GetTransform().GetWorldPosition().x - comp.GetPrevTargetPos().x);
        ecs::GetEntity(&comp)->GetTransform().SetWorldPosition({ cameraCompIter.GetEntity()->GetTransform().GetWorldPosition().x + (intensity * distDiff), currY });
    }
}

