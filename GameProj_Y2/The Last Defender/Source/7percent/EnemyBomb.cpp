/******************************************************************************/
/*!
\file   EnemyBomb.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   05/02/2025

\author Chua Wen Shing Bryan (100%)
\par    email: c.wenshingbryan\@digipen.edu
\par    DigiPen login: c.wenshingbryan

\brief
    Function definations for the Bomb that will be used by enemy type - SUICIDE


All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/

#include "EnemyBomb.h"
#include "ComponentLookupWorkaround.h"


EnemyBombComponent::EnemyBombComponent() : spawnedExplosion(false), bombRange(nullptr), bombTimer(3.f), bombPrimed(false), primedSpriteID(0), unprimedSpriteID(0)
                                          
#ifdef IMGUI_ENABLED
    , REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw)
#endif 
{
   
        
}

#ifdef IMGUI_ENABLED
void EnemyBombComponent::EditorDraw( EnemyBombComponent& comp)
{
    auto& unPrimed = ResourceManager::GetSprite(comp.GetUnPrimedSpriteID());
    auto& Primed = ResourceManager::GetSprite(comp.GetPrimedSpriteID());

    ImGui::Text("Drag an Sprite from the browser to assign Unprimed");

    // Existing sprite handling code...
    ImGui::ImageButton("UnPrimed bomb", VulkanManager::Get().VkTextureManager().getTexture(unPrimed.textureID).ImGui_handle, ImVec2(100, 100),
        ImVec2(unPrimed.texCoords.x, unPrimed.texCoords.y),
        ImVec2(unPrimed.texCoords.z, unPrimed.texCoords.w));
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SPRITE_ID"))
        {
            comp.SetUnPrimedSpriteID(*static_cast<size_t*>(payload->Data));
            if (ecs::CompHandle<RenderComponent> renderComp{ ecs::GetEntity(&comp)->GetComp<RenderComponent>() })
                renderComp->SetSpriteID(comp.GetUnPrimedSpriteID());

        }
        ImGui::EndDragDropTarget();
    }

    ImGui::ImageButton("Primed Bomb", VulkanManager::Get().VkTextureManager().getTexture(Primed.textureID).ImGui_handle, ImVec2(100, 100),
        ImVec2(Primed.texCoords.x, Primed.texCoords.y),
        ImVec2(Primed.texCoords.z, Primed.texCoords.w));
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SPRITE_ID"))
        {
            comp.SetPrimedSpriteID(*static_cast<size_t*>(payload->Data));
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::Text("Drag an Sprite from the browser to assign Pressed");
}
#endif

void EnemyBombComponent::SetBombPrimed(bool val) { bombPrimed = val; }
bool EnemyBombComponent::GetBombPrimed() { return bombPrimed; }

EntityReference& EnemyBombComponent::AttachedRange() { return bombRange; }
void EnemyBombComponent::SetBombCountdown(float val) { bombTimer = val; }
void EnemyBombComponent::SetPrimedSpriteID(size_t val) { primedSpriteID = val; }
void EnemyBombComponent::SetUnPrimedSpriteID(size_t val) { unprimedSpriteID = val; }
size_t EnemyBombComponent::GetPrimedSpriteID() { return primedSpriteID; }
size_t EnemyBombComponent::GetUnPrimedSpriteID() { return unprimedSpriteID; }

bool EnemyBombComponent::BombDenotated() 
{
    return (bombTimer <= 0.f) ? true : false;
}

void EnemyBombComponent::BombCountdown()
{
   if(bombPrimed) bombTimer -= GameTime::FixedDt();
}




EnemyBombSystem::EnemyBombSystem()
    : System_Internal(&EnemyBombSystem::UpdateEnemyBomb)
{
}


void EnemyBombSystem::UpdateEnemyBomb(EnemyBombComponent& comp)
{
    ecs::EntityHandle bombEntity{ ecs::GetEntity(&comp) };


    if (ecs::CompHandle<EnemyControllerComponent> enemyComp{ bombEntity->GetCompInParents<EnemyControllerComponent>() })
    {
        ecs::EntityHandle enemyEntity{ ecs::GetEntity(enemyComp) };
        ecs::CompHandle<HealthComponent> enemyHpComp{ enemyEntity->GetComp<HealthComponent>() };
        if (enemyHpComp->IsDead())
        {
            comp.SetBombCountdown(0.f);
        }
    }


    if (comp.GetBombPrimed())
    {
        comp.BombCountdown();
        if (ecs::CompHandle<RenderComponent> renderComp{ ecs::GetEntity(&comp)->GetComp<RenderComponent>() })
            renderComp->SetSpriteID(comp.GetPrimedSpriteID());

    }


    if (comp.BombDenotated())
    {
        bombEntity->GetTransform().SetParent(nullptr);
        ecs::EntityHandle rangeEntity = PrefabManager::LoadPrefab("BombRange");

        // Set its position to be same as comp entity's
        ecs::CompHandle<BombRangeComponent> bombRange = rangeEntity->GetComp<BombRangeComponent>();

        bombRange->Fire(
            ecs::GetEntity(&comp)->GetTransform().GetWorldPosition(),
            Vector2(0.0f),
            Vector2(300.0f),
            105,
            1000.0f,
            0.5f);

        ecs::DeleteEntity(ecs::GetEntity(&comp));


        ecs::DeleteEntity(bombEntity);
    }
}