#include "CheckBoxComponent.h"
#include "ResourceManager.h"
#include "RenderSystem.h"

/******************************************************************************/
/*!
\file   CheckBoxComponent.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   13/03/2025

\author Chua Wen Shing Bryan (100%)
\par    email: c.wenshingbryan\@digipen.edu
\par    DigiPen login: c.wenshingbryan

\brief
    Functions for the checkbox component to toggle fullscreen
    checkbox only toggles fullscreen for now 

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/

CheckBoxComponent::CheckBoxComponent() :
#ifdef IMGUI_ENABLED
    REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
    isChecked(false), spriteID_Pressed(0), spriteID_Unpressed(0), setInitial(false)
{

}

bool CheckBoxComponent::GetIsChecked() { return isChecked; }
void CheckBoxComponent::SetIsChecked(bool val) { isChecked = val; }
size_t	CheckBoxComponent::GetSpriteIDPressed() { return spriteID_Pressed; }
size_t  CheckBoxComponent::GetSpriteIDUnPressed() { return spriteID_Unpressed; }
void CheckBoxComponent::SetSpriteIDPressed(size_t val) { spriteID_Pressed = val; }
void CheckBoxComponent::SetSpriteIDUnPressed(size_t val){ spriteID_Unpressed = val;}
void CheckBoxComponent::SetInitialSpriteSet(bool val) { setInitial = val; }
bool CheckBoxComponent::GetInitialSpriteSet() { return setInitial; }

void CheckBoxComponent::OnAttached()
{
    SetIsChecked(static_cast<bool>(ST<GameSettings>::Get()->m_fullscreenMode));
}

void CheckBoxComponent::OnDetached()
{

}

#ifdef IMGUI_ENABLED
void CheckBoxComponent::EditorDraw(CheckBoxComponent& comp)
{
    auto& spritePressed = ResourceManager::GetSprite(comp.GetSpriteIDPressed());
    auto& spriteUnPressed = ResourceManager::GetSprite(comp.GetSpriteIDUnPressed());

    ImGui::Text("Drag an Sprite from the browser to assign Unpressed");

    // Existing sprite handling code...
    ImGui::ImageButton("Preview_UnPressed", VulkanManager::Get().VkTextureManager().getTexture(spriteUnPressed.textureID).ImGui_handle, ImVec2(100, 100),
        ImVec2(spriteUnPressed.texCoords.x, spriteUnPressed.texCoords.y),
        ImVec2(spriteUnPressed.texCoords.z, spriteUnPressed.texCoords.w));
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SPRITE_ID"))
        {
            comp.SetSpriteIDUnPressed(*static_cast<size_t*>(payload->Data));
            if (ecs::CompHandle<RenderComponent> renderComp{ ecs::GetEntity(&comp)->GetComp<RenderComponent>() })
                renderComp->SetSpriteID(comp.GetIsChecked() ? comp.GetSpriteIDPressed() : comp.GetSpriteIDUnPressed());

        }
        ImGui::EndDragDropTarget();
    }

    ImGui::ImageButton("Preview_Pressed", VulkanManager::Get().VkTextureManager().getTexture(spritePressed.textureID).ImGui_handle, ImVec2(100, 100),
        ImVec2(spritePressed.texCoords.x, spritePressed.texCoords.y),
        ImVec2(spritePressed.texCoords.z, spritePressed.texCoords.w));
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SPRITE_ID"))
        {
            comp.SetSpriteIDPressed(*static_cast<size_t*>(payload->Data));
        
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::Text("Drag an Sprite from the browser to assign Pressed");
}
#endif

CheckBoxSystem::CheckBoxSystem()
    : System_Internal(&CheckBoxSystem::UpdateCheckBoxComp)
{
}


void CheckBoxSystem::UpdateCheckBoxComp(CheckBoxComponent& comp)
{
    bool onClicked{ Input::GetKeyPressed(KEY::M_LEFT) };

    //Sets the initial sprite
    if (!comp.GetInitialSpriteSet())
    {
        comp.SetInitialSpriteSet(true);
    }

    if (ecs::CompHandle<RenderComponent> renderComp{ ecs::GetEntity(&comp)->GetComp<RenderComponent>() })
    {
        size_t currSpriteID = ST<GameSettings>::Get()->m_fullscreenMode ? comp.GetSpriteIDPressed() : comp.GetSpriteIDUnPressed();
        renderComp->SetSpriteID(currSpriteID);
    }

    if (!onClicked)
        return;

    // Get whether the cursor is on the button. We'll need this later.
    bool isCursorOnButton{ util::IsPointInside(Input::GetMousePosWorld(), ecs::GetEntityTransform(&comp)) };

    if (isCursorOnButton)
    {
     
        onCheckBoxClicked(comp);
    }
}

void CheckBoxSystem::onCheckBoxClicked(CheckBoxComponent& comp)
{
    
    comp.SetIsChecked(!comp.GetIsChecked());

    size_t currSpriteID = comp.GetIsChecked() ? comp.GetSpriteIDPressed() : comp.GetSpriteIDUnPressed();

    if (ecs::CompHandle<RenderComponent> renderComp{ ecs::GetEntity(&comp)->GetComp<RenderComponent>() })
        renderComp->SetSpriteID(currSpriteID);

    ST<GameSettings>::Get()->m_fullscreenMode = static_cast<int>(comp.GetIsChecked());
    ST<GameSettings>::Get()->ApplyFullscreen();
    ST<GameSettings>::Get()->Save();
}