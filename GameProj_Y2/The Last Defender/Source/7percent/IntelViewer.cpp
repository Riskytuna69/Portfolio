/******************************************************************************/
/*!
\file   IntelViewer.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   09/03/2025

\author Chan Kuan Fu Ryan (100%)
\par    email: c.kuanfuryan\@digipen.edu
\par    DigiPen login: c.kuanfuryan

\brief
  UI for viewing intel entries (short stories) collected throughout the game.
  Attached to pause scene.

All content © 2025 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#include "IntelViewer.h"
#include "Messaging.h"
#include "GameManager.h"
#include "PrefabManager.h"
#include "ResourceManager.h"

IntelViewerComponent::IntelViewerComponent() :
#ifdef IMGUI_ENABLED
    REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
    textSpawnPoint(nullptr),
    blockerPrefabName(""),
    numIntel{ 1 },
    vecIntel{ std::vector<EntityReference>(numIntel, nullptr) },
    mapBlockers{},
    textPrefab(nullptr)
{
}

#ifdef IMGUI_ENABLED
void IntelViewerComponent::EditorDraw(IntelViewerComponent& comp)
{
    comp.textSpawnPoint.EditorDraw("Text Spawn Point");

    static char buffer[256];
    strncpy_s(buffer, comp.blockerPrefabName.c_str(), sizeof(buffer) - 1);
    if (ImGui::InputText("Blocker Prefab", buffer, sizeof(buffer)))
    {
        comp.blockerPrefabName = buffer;
    };

    // Input field to modify numIntel
    ImGui::InputInt("Number of Containers", &comp.numIntel);

    // Clamp to 1
    if (comp.numIntel < 1)
    {
        comp.numIntel = 1;
    }

    // Add button to apply changes
    if (ImGui::Button("Resize"))
    {
        comp.vecIntel.resize(static_cast<size_t>(comp.numIntel));
    }

    // Draw input fields for EntityReferences
    int i = 0;
    for (EntityReference& ref : comp.vecIntel)
    {
        ref.EditorDraw(std::to_string(i++).c_str());
    }
}
#endif

IntelViewerSystem::IntelViewerSystem() : 
    System_Internal(&IntelViewerSystem::UpdateIntelViewerComp)
{
}

void IntelViewerSystem::OnAdded()
{
    Messaging::Subscribe("ViewIntel", IntelViewerSystem::ViewIntel);
    Messaging::Subscribe("CheckIntelUnlocks", IntelViewerSystem::CheckIntelUnlocks);
}

void IntelViewerSystem::OnRemoved()
{
    // Remember to unsubscribe messages
    Messaging::Unsubscribe("CheckIntelUnlocks", IntelViewerSystem::CheckIntelUnlocks);
    Messaging::Unsubscribe("ViewIntel", IntelViewerSystem::ViewIntel);
}

void IntelViewerSystem::CheckIntelUnlocks()
{
    // Is it really necessary for iteration?
    // Regardless of how you go about it, iteration is inevitable.
    // You will have to either iterate in the button broadcast using ecs events comp or here.
    // Even though there will only ever be one intel viewer component.
    for (auto it{ ecs::GetCompsBegin<IntelViewerComponent>() }, end{ ecs::GetCompsEnd<IntelViewerComponent>() }; it != end; ++it)
    {
        // Delete textPrefab if it already exists
        if (it->textPrefab)
        {
            ecs::DeleteEntity(it->textPrefab);
        }

        // Iterate through all intel
        for (int id = 0; id < it->vecIntel.size(); ++id)
        {
            // If intel not unlocked and no blocker at location
            if (!ST<GameManager>::Get()->GetIntelUnlocked(id) && !it->mapBlockers[id])
            {
                // Spawn blocker prefab
                ecs::EntityHandle blockerEntity = PrefabManager::LoadPrefab(it->blockerPrefabName);
                blockerEntity->GetTransform().SetParent(it->vecIntel[id]->GetTransform());
                blockerEntity->GetTransform().SetLocalPosition(Vector2(0.0f, 0.0f));

                // Assign to vecBlockers
                it->mapBlockers[id] = blockerEntity;
            }
            // If intel is unlocked but blocker already exists
            else if (ST<GameManager>::Get()->GetIntelUnlocked(id) && it->mapBlockers[id])
            {
                // Delete blocker entity
                ecs::DeleteEntity(it->mapBlockers[id]);

                // Assign to vecBlockers
                it->mapBlockers[id] = nullptr;
            }
        }
    }
}

void IntelViewerSystem::ViewIntel(int num)
{
    // First need to check if the intel is unlocked.
    if (!ST<GameManager>::Get()->GetIntelUnlocked(num))
    {
        // Maybe play a beep sound...

        return;
    }

    // Find the component and operate.
    for (auto it{ ecs::GetCompsBegin<IntelViewerComponent>() }, end{ ecs::GetCompsEnd<IntelViewerComponent>() }; it != end; ++it)
    {
        // If the text prefab already exists, delete it first.
        if (it->textPrefab)
        {
            ecs::DeleteEntity(it->textPrefab);
        }

        // Decoding the number to text entity is trivial.
        ecs::EntityHandle textEntity = PrefabManager::LoadPrefab(std::to_string(num));

        // If unable to load, just return
        if (!textEntity)
        {
            return;
        }

        // Assign to comp.
        it->textPrefab = textEntity;

        // Set parent.
        textEntity->GetTransform().SetParent(it->textSpawnPoint->GetTransform());

        // Set local position.
        textEntity->GetTransform().SetLocalPosition(Vector2(0.0f, 0.0f));
    }
}

void IntelViewerSystem::UpdateIntelViewerComp(IntelViewerComponent& comp)
{
    UNREFERENCED_PARAMETER(comp);
}
