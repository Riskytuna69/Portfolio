/******************************************************************************/
/*!
\file   Button.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   11/02/2024

\author Chua Wen Shing Bryan (25%)
\par    email: c.wenshingbryan\@digipen.edu
\par    DigiPen login: c.wenshingbryan

\author Ryan Cheong (35%)
\par    email: ngaihangryan.cheong\@digipen.edu
\par    DigiPen login: ngaihangryan.cheong

\author Kendrick Sim Hean Guan (40%)
\par    email: kendrickheanguan.s\@digipen.edu
\par    DigiPen login: kendrickheanguan.s

\brief
    Cpp file for definations of fuctions for button component.
    Button component allows any enitities that it is attached to become
    a button.
    It has a pressed and unpressed state and the png for both states can
    be changed

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/

#include "Button.h"
#include "Engine.h"

#include "FunctionQueue.h"
#include "SceneTransitionManager.h"
#include "UIScreenManager.h"
#include "Confirmation.h"
#include "ResourceManager.h"
#include "TweenManager.h"
#include "AudioManager.h"
#include "CameraComponent.h"
#include "Scheduler.h"

namespace fs = std::filesystem;

// Static map holding string-function pairs
static std::unordered_map<std::string, std::function<void(std::string)>> functionMap =
{
    { "LoadScene", [](const std::string& sceneToLoad)
    {
        std::string scenePath{ ST<Filepaths>::Get()->scenesSave + "/" + sceneToLoad + ".scene" };
        FunctionQueue::QueueOperation([scenePath]
        {
            ST<SceneManager>::Get()->LoadScene(scenePath);
        });
    }},
    { "UnloadScene", [](const std::string& sceneToUnload)
    {
        Scene* scene = ST<SceneManager>::Get()->GetSceneWithName(sceneToUnload);

        // If scene to unload doens't exist in the scene pool, return
        if (!scene)
        {
            return;
        }

        int index = scene->GetIndex();

        FunctionQueue::QueueOperation([index]
        {
            ST<SceneManager>::Get()->UnloadScene(index);
            ST<AudioManager>::Get()->ResumeAllSounds();
        });
    }},
    { "CloseGame", [](const std::string& none)
    {
        UNREFERENCED_PARAMETER(none);
        ST<Engine>::Get()->MarkToShutdown();
    }},
    { "StartGame", [](const std::string& none)
    {
        UNREFERENCED_PARAMETER(none);

        // Move the camera
        ecs::CompHandle<CameraComponent> cameraComp = ecs::GetCompsBegin<CameraComponent>().GetComp();
        ecs::EntityHandle cameraEntity = ecs::GetEntity(cameraComp);
        ST<TweenManager>::Get()->StartTween(
            cameraEntity,
            &Transform::SetWorldPosition,
            cameraEntity->GetTransform().GetWorldPosition(),
            cameraEntity->GetTransform().GetWorldPosition() + Vector2{ 0.0f, -860.0f },
            1.5f,
            TT::EASE_BOTH);

        // Disable all buttons
        for (ecs::EntityIterator entity = ecs::GetEntitiesBegin(); entity != ecs::GetEntitiesEnd(); ++entity)
        {
            if (auto eventsComp{ entity->GetComp < EntityEventsComponent>() })
            {
                eventsComp->BroadcastAll("DisableButtons");
            }
        }
        
        // Schedule scene transition manager call after 1.5f
        ST<Scheduler>::Get()->Add(1.5f, []
        {
            ST<SceneTransitionManager>::Get()->TransitionScene("MenuScene", "Cutscene Scene");
        });
    }},
    { "BroadcastMessage", [](const std::string& message)
    {
        // Broadcast to all other systems
        Messaging::BroadcastAll(message);
    }},
    { "ChangeMenuScreenByName", [](const std::string& nextScreenName)
    {
        for (auto uiScreenIter{ ecs::GetCompsBegin<UIScreenManagerComponent>() }, endIter{ ecs::GetCompsEnd<UIScreenManagerComponent>() }; uiScreenIter != endIter; ++uiScreenIter)
            uiScreenIter->SwitchToScreen(nextScreenName);
    }},
    { "ChangeMenuScreenToPrev", [](const std::string&)
    {
        for (auto uiScreenIter{ ecs::GetCompsBegin<UIScreenManagerComponent>() }, endIter{ ecs::GetCompsEnd<UIScreenManagerComponent>() }; uiScreenIter != endIter; ++uiScreenIter)
            uiScreenIter->SwitchToPrevScreen();
    }},
    { "OpenConfirmationDialog_ToMainMenu", [](const std::string&)
    {
        for (auto confirmIter{ ecs::GetCompsBegin<ConfirmationComponent>() }, endIter{ ecs::GetCompsEnd<ConfirmationComponent>() }; confirmIter != endIter; ++confirmIter)
            confirmIter->Init([]() {
                functionMap.at("UnloadAllScenes")("MenuScene");
                // functionMap.at("LoadScene")("ParallaxScene");
            });
        functionMap.at("ChangeMenuScreenByName")("Confirmation UI");
    }},
    { "OpenConfirmationDialog_QuitGame", [](const std::string&)
    {
        for (auto confirmIter{ ecs::GetCompsBegin<ConfirmationComponent>() }, endIter{ ecs::GetCompsEnd<ConfirmationComponent>() }; confirmIter != endIter; ++confirmIter)
            confirmIter->Init([]() {
                functionMap.at("CloseGame")("");
            });
        functionMap.at("ChangeMenuScreenByName")("Confirmation UI");
    }},
    { "ConfirmYes", [](const std::string&)
    {
        for (auto confirmIter{ ecs::GetCompsBegin<ConfirmationComponent>() }, endIter{ ecs::GetCompsEnd<ConfirmationComponent>() }; confirmIter != endIter; ++confirmIter)
            confirmIter->SelectYes();
    }},
    { "UpdateIntel", [](const std::string&)
    {
        Messaging::BroadcastAll("CheckIntelUnlocks");
    }},
    { "ViewIntel", [](const std::string& num)
    {
        Messaging::BroadcastAll("ViewIntel", std::stoi(num));
    }},
    { "UnloadAllScenes", [](const std::string& replacement)
    {
        std::string scenePath{ ST<Filepaths>::Get()->scenesSave + "/" + replacement + ".scene" };
        FunctionQueue::QueueOperation([scenePath]
        {
            ST<SceneManager>::Get()->UnloadAllScenes(scenePath);
            ST<AudioManager>::Get()->ResumeAllSounds();
        });
    }},
    { "StartTutorial", [](const std::string& none)
    {
        UNREFERENCED_PARAMETER(none);
        ST<SceneTransitionManager>::Get()->TransitionScene("MenuScene", "TutorialScene");
    }},
    { "TweenCameraDown", [](const std::string& none)
    {
        UNREFERENCED_PARAMETER(none);

        // Get the camera entity
        ecs::EntityHandle cameraEntity{ nullptr };
        cameraEntity = ecs::GetCompsActiveBegin<CameraComponent>().GetEntity();

		// Tween the camera down
        ST<TweenManager>::Get()->StartTween(
            cameraEntity,
            &Transform::SetWorldPosition,
            cameraEntity->GetTransform().GetWorldPosition(),
			Vector2{ 0.0f, 0.0f },
			1.6f,
			TT::EASE_BOTH
		);

		// Disable buttons for 1.6 seconds
        ST<Scheduler>::Get()->Add(0.0f, [] {
        for (ecs::EntityIterator entity = ecs::GetEntitiesBegin(); entity != ecs::GetEntitiesEnd(); ++entity)
        {
            if (auto eventsComp{ entity->GetComp < EntityEventsComponent>() })
            {
                eventsComp->BroadcastAll("DisableButtons");
            }
        }
		// Enable buttons after 1.6 seconds
        }).Then(1.6f, [] {
        for (ecs::EntityIterator entity = ecs::GetEntitiesBegin(); entity != ecs::GetEntitiesEnd(); ++entity)
        {
            if (auto eventsComp{ entity->GetComp < EntityEventsComponent>() })
            {
                eventsComp->BroadcastAll("EnableButtons");
            }
        }}
        );
    }},
    { "TweenCameraUp", [](const std::string& none)
    {
        UNREFERENCED_PARAMETER(none);

        // Get the camera entity
        ecs::EntityHandle cameraEntity{ nullptr };
        cameraEntity = ecs::GetCompsActiveBegin<CameraComponent>().GetEntity();

        // Tween the camera down
        ST<TweenManager>::Get()->StartTween(
            cameraEntity,
            &Transform::SetWorldPosition,
            cameraEntity->GetTransform().GetWorldPosition(),
            Vector2{ 0.0f, 540.0f },
            1.6f,
            TT::EASE_BOTH
        );

        // Disable buttons for 1.6 seconds
        ST<Scheduler>::Get()->Add(0.0f, [] {
        for (ecs::EntityIterator entity = ecs::GetEntitiesBegin(); entity != ecs::GetEntitiesEnd(); ++entity)
        {
            if (auto eventsComp{ entity->GetComp < EntityEventsComponent>() })
            {
                eventsComp->BroadcastAll("DisableButtons");
            }
        }
        // Enable buttons after 1.6 seconds
        }).Then(1.6f, [] {
            for (ecs::EntityIterator entity = ecs::GetEntitiesBegin(); entity != ecs::GetEntitiesEnd(); ++entity)
            {
                if (auto eventsComp{ entity->GetComp < EntityEventsComponent>() })
                {
                    eventsComp->BroadcastAll("EnableButtons");
                }
            }}
        );
    } },
};

ButtonComponent::ButtonComponent() :
#ifdef IMGUI_ENABLED
    REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
    numFunctions    { 1 }
    , vecFunctions  { static_cast<size_t>(numFunctions), { "", "" } }
    , wasPressed{ false }
    , isPressTarget{ false }
    , spriteID_Pressed{ 0 }
    , spriteID_Unpressed{ 1 }
    , clickSound{ "Click" }
    , isPressable{ true }
{

}

bool ButtonComponent::GetIsPressTarget() const
{
    return isPressTarget;
}

void ButtonComponent::SetIsPressTarget(bool newIsPressTarget)
{
    isPressTarget = newIsPressTarget;
}

void ButtonComponent::SetIsPressed(bool isPressed)
{
    // Updates the render component's sprite. Could consider caching isPressed so we don't call this if we don't need to, but unlikely that this would cause performance issues.
    if (ecs::CompHandle<RenderComponent> renderComp{ ecs::GetEntity(this)->GetComp<RenderComponent>() })
        renderComp->SetSpriteID(isPressed ? spriteID_Pressed : spriteID_Unpressed);

    // When the key is released, play click sound
    if (isPressed && !wasPressed)
    {
        ST<AudioManager>::Get()->StartGroupedSound(clickSound);
    }
    wasPressed = isPressed;
}

size_t	ButtonComponent::GetSpriteIDPressed() {
    return spriteID_Pressed;
}
size_t  ButtonComponent::GetSpriteIDUnPressed() {
    return spriteID_Unpressed;
}

void ButtonComponent::SetSpriteIDPressed(size_t val)
{
    spriteID_Pressed = val;
}

void ButtonComponent::SetSpriteIDUnPressed(size_t val)
{
    spriteID_Unpressed = val;
}

bool ButtonComponent::GetIsPressable() const
{
    return isPressable;
}

void ButtonComponent::Enable()
{
    isPressable = true;
}

void ButtonComponent::Disable()
{
    isPressable = false;
}

void ButtonComponent::OnAttached()
{
    ecs::GetEntity(this)->GetComp<EntityEventsComponent>()->Subscribe("DisableButtons",this, &ButtonComponent::Disable);
    ecs::GetEntity(this)->GetComp<EntityEventsComponent>()->Subscribe("EnableButtons",this, &ButtonComponent::Enable);
}

void ButtonComponent::OnDetached()
{
    if (auto eventsComp{ ecs::GetEntity(this)->GetComp<EntityEventsComponent>() })
    {
        eventsComp->Unsubscribe("DisableButtons",this, &ButtonComponent::Disable);
        eventsComp->Unsubscribe("EnableButtons",this, &ButtonComponent::Enable);
    }
}

#ifdef IMGUI_ENABLED
void ButtonComponent::EditorDraw(ButtonComponent& comp)
{
    auto& spritePressed = ResourceManager::GetSprite(comp.GetSpriteIDPressed());
    auto& spriteUnPressed = ResourceManager::GetSprite(comp.GetSpriteIDUnPressed());

    ImGui::Text("Drag an Sprite from the browser to assign Unpressed");

    // Existing sprite handling code...
    ImGui::ImageButton("Preview_UnPressed", VulkanManager::Get().VkTextureManager().getTexture(spriteUnPressed.textureID).ImGui_handle, ImVec2(100, 100),
                       ImVec2(spriteUnPressed.texCoords.x, spriteUnPressed.texCoords.y),
                       ImVec2(spriteUnPressed.texCoords.z, spriteUnPressed.texCoords.w));
    if(ImGui::BeginDragDropTarget())
    {
        if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SPRITE_ID"))
        {
            comp.SetSpriteIDUnPressed(*static_cast<size_t*>(payload->Data));
            if(ecs::CompHandle<RenderComponent> renderComp{ ecs::GetEntity(&comp)->GetComp<RenderComponent>() })
                renderComp->SetSpriteID(comp.GetSpriteIDUnPressed());
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::ImageButton("Preview_Pressed", VulkanManager::Get().VkTextureManager().getTexture(spritePressed.textureID).ImGui_handle, ImVec2(100, 100),
                       ImVec2(spritePressed.texCoords.x, spritePressed.texCoords.y),
                       ImVec2(spritePressed.texCoords.z, spritePressed.texCoords.w));
    if(ImGui::BeginDragDropTarget())
    {
        if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SPRITE_ID"))
        {
            comp.SetSpriteIDPressed(*static_cast<size_t*>(payload->Data));
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::Text("Drag an Sprite from the browser to assign Pressed");

    // Button Functions
    ImGui::Separator();
    ImGui::Text("Button Functions");

    // Input field to modify numFunctions
    ImGui::InputInt("Number of Functions", &comp.numFunctions);

    // Clamp to 1
    if (comp.numFunctions < 1)
    {
        comp.numFunctions = 1;
    }

    // Add button to apply changes
    if (ImGui::Button("Resize"))
    {
        comp.vecFunctions.resize(static_cast<size_t>(comp.numFunctions));
    }

    // Draw pairs as input fields
    int id = 0;
    auto DrawFunctionInput = [&id](std::pair<std::string, std::string>& pair)
    {
        ImGui::Separator();
        ImGui::PushID(id++);

        // Function combo box
        if (ImGui::BeginCombo("Function", pair.first.c_str()))
        {
            for (auto const& entry : functionMap)
            {
                bool isSelected = (entry.first == pair.first);
                if (ImGui::Selectable(entry.first.c_str(), isSelected))
                {
                    pair.first = entry.first;
                }

                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        // These functions handle scenes, have fixed dropdown of params
        if (pair.first == "LoadScene" || pair.first == "UnloadScene")
        {
            // Display current scene path
            const std::string& currentPath = pair.second;
            ImGui::Text("Target Scene: %s", currentPath.empty() ? "None" : currentPath.c_str());

            // Scene selection button
            if (ImGui::Button("Select Scene"))
            {
                ImGui::OpenPopup("SceneSelectPopup");
            }

            if (ImGui::BeginPopup("SceneSelectPopup"))
            {
                for (const auto& entry : fs::directory_iterator{ ST<Filepaths>::Get()->scenesSave })
                {
                    if (entry.path().extension() != ".scene")
                        continue;

                    std::string sceneName{ entry.path().stem().string() };
                    if (ImGui::MenuItem(sceneName.c_str()))
                    {
                        pair.second = sceneName;
                    }
                }
                ImGui::EndPopup();
            }
        }
        // These functions don't need params
        else if (
            pair.first == "CloseGame" || 
            pair.first == "StartGame" || 
            pair.first == "StartTutorial" ||
            pair.first == "TweenCameraDown" ||
            pair.first == "TweenCameraUp")
        {
            ImGui::Text("No parameters required");
        }
        // These functions have 1-string params
        else
        {
            static char stringBuffer[256];
            strncpy_s(stringBuffer, pair.second.c_str(), sizeof(stringBuffer) - 1);
            if (ImGui::InputText("Parameter", stringBuffer, sizeof(stringBuffer)))
            {
                pair.second = stringBuffer;
            }
        }
        ImGui::PopID();
    };
    for (auto& val : comp.vecFunctions)
    {
        DrawFunctionInput(val);
    }

    comp.clickSound.EditorDraw("Click Sound");
}
#endif

void ButtonComponent::Serialize(Serializer& writer) const
{
    ISerializeable::Serialize(writer);

    writer.StartArray("vecFunctions");
    for (const auto& pair : vecFunctions)
    {
        writer.StartObject();
        writer.Serialize("function", pair.first);
        writer.Serialize("parameter", pair.second);
        writer.EndObject();
    }
    writer.EndArray();

}
void ButtonComponent::Deserialize(Deserializer& reader)
{
    ISerializeable::Deserialize(reader);

    size_t numElems{};
    reader.GetArraySize("vecFunctions", &numElems);
    reader.PushAccess("vecFunctions");
    vecFunctions.clear();

    for (size_t i{}; i < numElems; i++)
    {
        reader.PushArrayElementAccess(i);
        std::pair<std::string, std::string> temp;
        reader.DeserializeVar("function", &temp.first);
        reader.DeserializeVar("parameter", &temp.second);
        vecFunctions.push_back(std::move(temp));
        reader.PopAccess();
    }
    reader.PopAccess();
}

ButtonSystem::ButtonSystem()
    : System_Internal(&ButtonSystem::UpdateButtonComp)
    , isPressed{}
    , isCurrPressed{}
    , isReleased{}
{
}

bool ButtonSystem::PreRun()
{
    isPressed = Input::GetKeyPressed(KEY::M_LEFT);
    isCurrPressed = Input::GetKeyCurr(KEY::M_LEFT);
    isReleased = Input::GetKeyReleased(KEY::M_LEFT);
    return true;
}

void ButtonSystem::UpdateButtonComp(ButtonComponent& comp)
{
    if (!comp.GetIsPressable())
        return;

    // Get whether the cursor is on the button. We'll need this later.
    bool isCursorOnButton{ util::IsPointInside(Input::GetMousePosWorld(), ecs::GetEntityTransform(&comp)) };

    // Update the color override of buttons based on hover state
    if (ecs::CompHandle<RenderComponent> renderComp{ ecs::GetEntity(&comp)->GetComp<RenderComponent>() })
    {
        float intensity{ isCursorOnButton && !isCurrPressed ? 1.3f : 1.0f };
        auto materialParams{ renderComp->m_materialInstance.getOverrideParameters() };
        materialParams.baseColor = Vector4{ intensity, intensity, intensity, 1.0f };
        renderComp->m_materialInstance.setParameterOverrides(materialParams);
    }
    
    // Do nothing if there was no event and this button isn't the press target.
    if (!(isPressed || isReleased || comp.GetIsPressTarget()))
        return;

    // Check if the input was pressed so we can update the press target.
    if (isPressed)
        // This button is only a press target if it was pressed on.
        comp.SetIsPressTarget(isCursorOnButton);

    // At this point we only care about buttons that are the target.
    // All buttons that are not the target are not pressed.
    if (!comp.GetIsPressTarget())
        return;
    
    // Now check if the input was released.
    if (isReleased)
    {
        // All buttons are not the target nor pressed when the input is released.
        comp.SetIsPressTarget(false);
        comp.SetIsPressed(false);

        // If input was released on the button, time to do action.
        if (isCursorOnButton)
            OnButtonClicked(comp);
    }
    // Input is still being held. (if there's a button that is a target, that means there is still input)
    else
        // Set the state of the targeted button based on if the mouse cursor is on/off the button.
        comp.SetIsPressed(isCursorOnButton);
}

void ButtonSystem::OnButtonClicked(ButtonComponent& comp)
{
    // Go down the vector of functions
    for (auto const& pair : comp.vecFunctions)
    {
        // Check if function exists
        if (functionMap.find(pair.first) == functionMap.end())
        {
            return;
        }
        // Call the associate function with parameter
        functionMap[pair.first](pair.second);
    }
}
