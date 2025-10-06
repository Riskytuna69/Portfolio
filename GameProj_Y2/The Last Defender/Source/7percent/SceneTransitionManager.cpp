/******************************************************************************/
/*!
\file   SceneTransitionManager.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   03/09/2025

\author Matthew Chan Shao Jie (100%)
\par    email: m.chan\@digipen.edu
\par    DigiPen login: m.chan

\brief
  Implementation of scene transitions

All content � 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#include "pch.h"
#include "ECS.h"
#include "SceneTransitionManager.h"
#include "TweenManager.h"
#include "FunctionQueue.h"
#include "ResourceManager.h"
#include "SceneManagement.h"
#include "CameraComponent.h"
#include "RenderComponent.h"
#include "AudioManager.h"

std::string ScenePath(std::string name)
{
    return{ ST<Filepaths>::Get()->scenesSave + "/" + name + ".scene" };
}

void SceneTransitionManager::TransitionScene([[maybe_unused]] std::string currSceneName, std::string nextSceneName)
{
    for (ecs::EntityIterator entity = ecs::GetEntitiesBegin(); entity != ecs::GetEntitiesEnd(); ++entity)
    {
        if (auto eventsComp{ entity->GetComp < EntityEventsComponent>() })
        {
            eventsComp->BroadcastAll("DisableButtons");
        }
    }

    std::vector<int> scenesToUnload;
    // Unload all scenes except transition scene
    for (auto sceneIter{ ST<SceneManager>::Get()->GetScenesBegin() },
        endIter{ ST<SceneManager>::Get()->GetScenesEnd() };
        sceneIter != endIter; ++sceneIter)
    {
        // Store the scene index to unload
        scenesToUnload.push_back(sceneIter->GetIndex());
    }

    // Schedule functions
    ST<Scheduler>::Get()->Add(0.0f, []
    {
        // Get scene manager
        SceneManager* sceneManager = ST<SceneManager>::Get();
        Scene* transitionScene = sceneManager->GetSceneWithName("TransitionScene");

		// If transition scene does not exist, load it
        if (!transitionScene)
        {
            ST<SceneManager>::Get()->LoadScene(ScenePath("TransitionScene"));
            transitionScene = sceneManager->GetSceneWithName("TransitionScene");
        }

        // Tween fade to black
        ecs::EntityHandle bgEntity = transitionScene->GetEntitiesBegin().GetEntity();
        ecs::CompHandle<RenderComponent> renderComp = bgEntity->GetComp<RenderComponent>();
        Vector4 initialColor = renderComp->GetColor(), finalColor = initialColor;
        finalColor.w = 1.0f;
        ST<TweenManager>::Get()->StartTween(
            bgEntity,
            &RenderComponent::SetColor,
            initialColor,
            finalColor,
            1.5f,
            TT::EASE_BOTH);

        // Interpolate volumes
        ST<AudioManager>::Get()->InterpolateGroupVolume(0.0f, 1.45f, "BGM");
        ST<AudioManager>::Get()->InterpolateGroupVolume(0.0f, 1.45f, "SFX");

    }).Then(1.5f, [nextSceneName, scenesToUnload]
    {
        // Stop all sounds
        ST<AudioManager>::Get()->StopAllSounds();

        // Get scene manager
        SceneManager* sceneManager = ST<SceneManager>::Get();

        // Unload the scenes
        for (auto sceneIndex : scenesToUnload)
        {
            ST<SceneManager>::Get()->UnloadScene(sceneIndex);
        }

        // Load the next scene
        sceneManager->LoadScene(ScenePath(nextSceneName));

		// Tween the audio volume back to normal
        ST<AudioManager>::Get()->InterpolateGroupVolume(ST<AudioManager>::Get()->GetGroupVolume("BGM"), 1.5f, "BGM");
        ST<AudioManager>::Get()->InterpolateGroupVolume(ST<AudioManager>::Get()->GetGroupVolume("SFX"), 1.5f, "SFX");

		// Try to get transition scene
        Scene* transitionScene = sceneManager->GetSceneWithName("TransitionScene");

        // If transition scene does not exist, load it
        if (!transitionScene)
        {
            ST<SceneManager>::Get()->LoadScene(ScenePath("TransitionScene"));
            transitionScene = sceneManager->GetSceneWithName("TransitionScene");
        }

		// Tween fade out of black
        ecs::EntityHandle bgEntity = transitionScene->GetEntitiesBegin().GetEntity();
        ecs::CompHandle<RenderComponent> renderComp = bgEntity->GetComp<RenderComponent>();
        Vector4 initialColor = renderComp->GetColor(), finalColor = initialColor;
        finalColor.w = 0.0f;
        ST<TweenManager>::Get()->StartTween(
            bgEntity,
            &RenderComponent::SetColor,
            initialColor,
            finalColor,
            1.5f,
            TT::EASE_BOTH);
    }
    ).Then(1.5f, []
    {
        SceneManager* sceneManager = ST<SceneManager>::Get();
        Scene* transitionScene = sceneManager->GetSceneWithName("TransitionScene");
        if (!transitionScene) return;
        sceneManager->UnloadScene(transitionScene->GetIndex());
    });
}
