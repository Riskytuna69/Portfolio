/******************************************************************************/
/*!
\file   GameOver.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   27/11/2024

\author Chan Kuan Fu Ryan (100%)
\par    email: c.kuanfuryan\@digipen.edu
\par    DigiPen login: c.kuanfuryan

\brief
  Component and system to manage game win/loss conditions as well as giving the
  option to restart the level when the session has ended.

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#include "GameOver.h"
#include "TweenManager.h"
#include "AudioManager.h"
#include "NameComponent.h"
#include "SceneManagement.h"
#include "GameSettings.h"
#include "TextComponent.h"
#include "GameManager.h"
#include "Player.h"
#include "Health.h"
#include "SceneSoundManager.h"
#include "Bullet.h"
#include "MultiReference.h"
#include "PrefabManager.h"

GameOverComponent::GameOverComponent() :
#ifdef IMGUI_ENABLED
    REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
    duration_FadeIn{ 1.0f }
    , duration_FadeOut{ 1.0f }
    , duration_LoseWait{ 1.0f }
    , duration_WinTextMovingApart{ 1.0f }
    , duration_WinScoreboardAppearing{ 1.0f }
    , duration_PerScore{ 0.0f }
    , gameStatus_YOffset{ 0.0f }
    , gameStatus_ScaleMultiplier{ 0.0f }
    , spacebar_YOffset{ 0.0f }
    , spacebar_ScaleMultiplier{ 0.0f }
	, score_YOffset{ 0.0f }
	, score_YInterval{ 0.0f }
	, scoreboardPosition{ nullptr }
    , text_Spacebar{ nullptr }
    , text_GameStatus{ nullptr }
    , vecScores{}
    , currentScoreID{ 0 }
    , internalTimer{ 0.0f }
    , scoreTimer{ 0.0f }
    , text_Spacebar_DefaultLocalPosition{}
    , text_GameStatus_DefaultLocalPosition{}
    , inited{ false }
    , playState{ PLAYSTATE::PLAY }
{
}

PLAYSTATE GameOverComponent::GetPlayState()
{
    return playState;
}

void GameOverComponent::SetPlayState(PLAYSTATE state)
{
    playState = state;
}

void GameOverComponent::Init()
{
    if (!text_Spacebar || !text_GameStatus)
    {
        return;
    }
    text_Spacebar->SetActive(true);
    text_GameStatus->SetActive(true);
    text_Spacebar_DefaultLocalPosition = text_Spacebar->GetTransform().GetLocalPosition();
    text_GameStatus_DefaultLocalPosition = text_GameStatus->GetTransform().GetLocalPosition();
    // Create score multiRefs
    std::vector<std::string> scoreNames
    {
        "Enemies Killed",
        "Player Deaths",
        "Intel Found",
        "Weapons Found",
        "Damage Shielded",
        "Time Elapsed",
        "Total Score"
    };

    // Create scorenames
    if (scoreboardPosition) // Make sure we don't crash...
    {
        for (size_t i = 0; i < scoreNames.size(); ++i)
        {
            auto score = ST<PrefabManager>::Get()->LoadPrefab("Score");
            vecScores.push_back(score);
            score->GetTransform().SetParent(scoreboardPosition->GetTransform());
            score->GetTransform().SetLocalPosition({ 0.0f, static_cast<float>(i) * score_YInterval });
            auto multiRef = score->GetComp<MultiReferenceComponent>()->GetReferences();

            // A little hard coded, sorry...
            multiRef.resize(3);
            auto children = score->GetTransform().GetChildren();

            // Assign children to vector so instantiated prefabs can keep their entity references
            for (auto child : children)
            {
                // Convert name to index number
                ecs::EntityHandle childEntity = child->GetEntity();
                int idx = std::stoi(childEntity->GetComp<NameComponent>()->GetName());
                multiRef[idx] = childEntity;
                
                // Also get text component and set all to transparent
                childEntity->GetComp<TextComponent>()->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f });
            }

            // Set name for score
            multiRef[0]->GetComp<TextComponent>()->SetText(scoreNames[i]);

            // Assign back to component
            score->GetComp<MultiReferenceComponent>()->SetReferences(multiRef);
        }
    }

    // Create other values
    duration_PerScore = duration_WinScoreboardAppearing / static_cast<float>(vecScores.size());
    currentScoreID = 0;
    scoreTimer = duration_PerScore + .1f; // A little bit of a hack to make it start one tween first
    inited = true;

    // Audio should be set to BGM group
    ST<AudioManager>::Get()->SetChannelGroup("GameOver", "BGM");
    ST<AudioManager>::Get()->SetChannelGroup("GameWin", "BGM");
}

#ifdef IMGUI_ENABLED
void GameOverComponent::EditorDraw(GameOverComponent& comp)
{
    // Float values
    ImGui::DragFloat("Fade In Duration", &comp.duration_FadeIn);
    ImGui::DragFloat("Fade Out Duration", &comp.duration_FadeOut);
    ImGui::DragFloat("Lose Wait Duration", &comp.duration_LoseWait);
    ImGui::DragFloat("Win Text Moving Apart Duration", &comp.duration_WinTextMovingApart);
    ImGui::DragFloat("Win Scoreboard Appearing Duration", &comp.duration_WinScoreboardAppearing);

    // WIN_ANIM_2
    ImGui::DragFloat("Game Status Y Offset", &comp.gameStatus_YOffset);
    ImGui::DragFloat("Game Status Scale Multiplier", &comp.gameStatus_ScaleMultiplier);
    ImGui::DragFloat("Spacebar Y Offset", &comp.spacebar_YOffset);
    ImGui::DragFloat("Spacebar Scale Multiplier", &comp.spacebar_ScaleMultiplier);

    // WIN_ANIM_3
    ImGui::DragFloat("Score Y Offset", &comp.score_YOffset);
    ImGui::DragFloat("Score Y Interval", &comp.score_YInterval);
    comp.scoreboardPosition.EditorDraw("Scoreboard Entity");

    // Must have text component!
    comp.text_Spacebar.EditorDraw("Text Spacebar");
    comp.text_GameStatus.EditorDraw("Text Game Status");
}
#endif

GameOverSystem::GameOverSystem()
    : System_Internal{ &GameOverSystem::UpdateGameOver }
{
}

void GameOverSystem::UpdateGameOver(GameOverComponent& comp)
{
    // If not initialised, try to get
    if (!comp.inited)
    {
        comp.Init();
    }

    // Get entity
    ecs::EntityHandle entity = ecs::GetEntity(&comp);

    // Hack commands
    /*
    if (Input::GetKeyPressed(KEY::Q))
    {
        Messaging::BroadcastAll("GameWin");
    }
    */
    
    // Different behaviours based on state
    switch (comp.GetPlayState())
    {
	case PLAYSTATE::PLAY:
    {
		break;
    }
    case PLAYSTATE::END_ANIM_LOSE_1:
    {
        comp.internalTimer += GameTime::FixedDt();

        // At the end of this anim, player will be teleported
        if (comp.internalTimer > comp.duration_FadeIn)
        {
            comp.SetPlayState(PLAYSTATE::END_ANIM_LOSE_2);
            comp.internalTimer = 0.0f;
        }
        break;
    }
    case PLAYSTATE::END_ANIM_LOSE_2:
    {
        comp.internalTimer += GameTime::FixedDt();

        // At the end of this wait, black fades out and text moves away
        if (comp.internalTimer > comp.duration_LoseWait)
        {
            comp.SetPlayState(PLAYSTATE::PLAY);
            comp.internalTimer = 0.0f;

            // Get current checkpoint
            ecs::EntityHandle currentCheckpoint = ST<GameManager>::Get()->GetCurrentCheckpoint();

            // Fallback on the old respawn system if there has been no checkpoint hit, for safety
            if (currentCheckpoint == nullptr)
            {
                int sceneIndex = ST<SceneManager>::Get()->GetEntitySceneIndex(ecs::GetEntity(&comp));
                ST<SceneManager>::Get()->ReloadScene(sceneIndex);
                return;
            }

            // New respawn here
            ecs::CompHandle<PlayerComponent> playerComp = ecs::GetCompsBegin<PlayerComponent>().GetComp();
            ecs::EntityHandle playerEntity = ecs::GetEntity(playerComp);
            playerEntity->GetTransform().SetWorldPosition(currentCheckpoint->GetTransform().GetWorldPosition());

            // Reset Player Health
            ecs::CompHandle<HealthComponent> healthComp = playerEntity->GetComp<HealthComponent>();
            healthComp->SetHealth(healthComp->GetMaxHealth());
            healthComp->UpdateIsDeadBroadcasted(false);
            healthComp->SetInvulnerable(1.0f);

            // Tell player death manager that player should appear
            Messaging::BroadcastAll("PlayerRespawned");

            // Get game status position
            Vector2 pos_text_GameStatus = comp.text_GameStatus->GetTransform().GetLocalPosition();

            // Tween game over status (died or win)
            ST<TweenManager>::Get()->StartTween(
                comp.text_GameStatus,
                &Transform::SetLocalPosition,
                pos_text_GameStatus,
                Vector2{ -comp.text_GameStatus_DefaultLocalPosition.x, pos_text_GameStatus.y },
                comp.duration_FadeOut,
                TT::EASE_BOTH);

            // Fade out of black
            ST<TweenManager>::Get()->StartTween(
                entity,
                &RenderComponent::SetColor,
                Vector4{ 0.0f, 0.0f, 0.0f, 1.0f },
                Vector4{ 0.0f, 0.0f, 0.0f, 0.0f },
                comp.duration_FadeOut,
                TT::EASE_IN);

            // Resubscribe messages
            OnAdded();

            // Restart sounds (also restores volumes)
            ecs::CompHandle< SceneSoundManagerComponent> sceneSoundManager = ecs::GetCompsBegin<SceneSoundManagerComponent>().GetComp();
            sceneSoundManager->Init();
        }
        break;
    }
    case PLAYSTATE::END_ANIM_WIN_1:
    {
        comp.internalTimer += GameTime::FixedDt();

        // At the end of this anim, text will move apart
        if (comp.internalTimer > comp.duration_FadeIn)
        {
            comp.SetPlayState(PLAYSTATE::END_ANIM_WIN_2);
            comp.internalTimer = 0.0f;

            // Get positions and scales
            Vector2 pos_text_GameStatus = comp.text_GameStatus->GetTransform().GetLocalPosition();
            Vector2 scale_text_GameStatus = comp.text_GameStatus->GetTransform().GetLocalScale();
            Vector2 pos_text_Spacebar = comp.text_Spacebar->GetTransform().GetLocalPosition();
            Vector2 scale_text_Spacebar = comp.text_Spacebar->GetTransform().GetLocalScale();

            // Translate game over status
            ST<TweenManager>::Get()->StartTween(
                comp.text_GameStatus,
                &Transform::SetLocalPosition,
                pos_text_GameStatus,
                Vector2{ 0.0f, pos_text_GameStatus.y + comp.gameStatus_YOffset },
                comp.duration_WinTextMovingApart,
                TT::EASE_BOTH);

            // Scale game over status
            ST<TweenManager>::Get()->StartTween(
                comp.text_GameStatus,
                &Transform::SetLocalScale,
                scale_text_GameStatus,
                scale_text_GameStatus * comp.gameStatus_ScaleMultiplier,
                comp.duration_WinTextMovingApart,
                TT::EASE_BOTH);

            // Translate spacebar
            ST<TweenManager>::Get()->StartTween(
                comp.text_Spacebar,
                &Transform::SetLocalPosition,
                pos_text_Spacebar,
                Vector2{ 0.0f, pos_text_Spacebar.y + comp.spacebar_YOffset },
                comp.duration_WinTextMovingApart,
                TT::EASE_BOTH);

            // Scale spacebar
            ST<TweenManager>::Get()->StartTween(
                comp.text_Spacebar,
                &Transform::SetLocalScale,
                scale_text_Spacebar,
                scale_text_Spacebar * comp.spacebar_ScaleMultiplier,
                comp.duration_WinTextMovingApart,
                TT::EASE_BOTH);
        }
        break;
    }
    case PLAYSTATE::END_ANIM_WIN_2:
    {
        comp.internalTimer += GameTime::FixedDt();

        // At the end of this anim, scoreboard will begin appearing
        if (comp.internalTimer > comp.duration_WinTextMovingApart)
        {
            comp.SetPlayState(PLAYSTATE::END_ANIM_WIN_3);
            comp.internalTimer = 0.0f;
        }

        break;
    }
    case PLAYSTATE::END_ANIM_WIN_3:
    {
        comp.internalTimer += GameTime::FixedDt();
        comp.scoreTimer += GameTime::FixedDt();

        // At the end of this anim, scoreboard is finished appearing
        if (comp.internalTimer > comp.duration_WinScoreboardAppearing)
        {
            comp.SetPlayState(PLAYSTATE::PENDING_RESTART);
            comp.internalTimer = 0.0f;
        }

        // Make scores appear
        if (comp.scoreTimer > comp.duration_PerScore)
        {
            if (comp.currentScoreID >= static_cast<int>(comp.vecScores.size()))
            {
                return;
            }

            auto scoreEntity = comp.vecScores[comp.currentScoreID];
            auto multiRef = scoreEntity->GetComp<MultiReferenceComponent>()->GetReferences();
            Vector2 currentLocalPosition = scoreEntity->GetTransform().GetLocalPosition();

            // Tween the scoreEntity to go down slightly
            ST<TweenManager>::Get()->StartTween(
                scoreEntity,
                &Transform::SetLocalPosition,
                currentLocalPosition,
                Vector2{ currentLocalPosition.x, currentLocalPosition.y + comp.score_YOffset },
                comp.duration_PerScore,
                TT::EASE_BOTH);

            // Iterate through all text component and tween their color to fade in
            for (size_t i = 0; i < multiRef.size(); ++i)
            {
                Vector4 currentColor = multiRef[i]->GetComp<TextComponent>()->GetColor();

                ST<TweenManager>::Get()->StartTween(
                    multiRef[i],
                    &TextComponent::SetColor,
                    currentColor,
                    Vector4{ currentColor.x, currentColor.y, currentColor.z, 1.0f },
                    comp.duration_PerScore,
                    TT::EASE_BOTH);
            }
            ++comp.currentScoreID;
            comp.scoreTimer = 0.0f;
        }
        break;
    }
    case PLAYSTATE::PENDING_RESTART:
    {
        if (Input::GetKeyPressed(KEY::SPACE))
        {
            int sceneIndex = ST<SceneManager>::Get()->GetEntitySceneIndex(ecs::GetEntity(&comp));
            ST<SceneManager>::Get()->ReloadScene(sceneIndex);
        }
        break;
    }
    default:
    {
        break;
    }
    }
}

// Called once when the game is loaded
void GameOverSystem::OnAdded()
{
    Messaging::Subscribe("DiedPlayer", OnDeath);
    Messaging::Subscribe("GameWin", OnGameWin);
}

// Called once when the game is unloaded
void GameOverSystem::OnRemoved()
{
    Messaging::Unsubscribe("DiedPlayer", OnDeath);
    Messaging::Unsubscribe("GameWin", OnGameWin);
}

// Called once when player dies
void GameOverSystem::OnDeath()
{
    // Get component
    auto comp = ecs::GetCompsBegin<GameOverComponent>();
    if (comp == ecs::GetCompsEnd<GameOverComponent>())
    {
        CONSOLE_LOG(LEVEL_WARNING) << "Lose condition triggered, but no GameOverComponent in scene!";
        return;
    }

    // Get entity
    ecs::EntityHandle entity = comp.GetEntity();

    // Sound management
    ST<AudioManager>::Get()->StopAllSounds();
    ST<AudioManager>::Get()->StartSingleSound("GameOver", false, std::nullopt, 2.0f);

	// Set the game state to defeat
    comp->SetPlayState(PLAYSTATE::END_ANIM_LOSE_1);
    comp->text_GameStatus->GetTransform().SetLocalPosition(comp->text_GameStatus_DefaultLocalPosition);
    auto textComp = comp->text_GameStatus->GetComp<TextComponent>();
    textComp->SetText("You Died!");
    textComp->SetColor({ 1.0f, 0.2f, 0.2f, 1.0f });

    // Do END_ANIM_LOSE_1
    Vector2 pos_text_Spacebar = comp->text_Spacebar->GetTransform().GetLocalPosition();
    Vector2 pos_text_GameStatus = comp->text_GameStatus->GetTransform().GetLocalPosition();

    // Tween game over status (died or win)
    ST<TweenManager>::Get()->StartTween(
        comp->text_GameStatus,
        &Transform::SetLocalPosition,
        pos_text_GameStatus,
        Vector2{ 0.0f, pos_text_GameStatus.y },
        comp->duration_FadeIn,
        TT::EASE_BOTH);

    // Fade in to black
    ST<TweenManager>::Get()->StartTween(
        entity,
        &RenderComponent::SetColor,
        Vector4{ 0.0f, 0.0f, 0.0f, 0.0f },
        Vector4{ 0.0f, 0.0f, 0.0f, 1.0f },
        comp->duration_FadeIn,
        TT::EASE_IN);

    // Fade out the SFX
    ST<AudioManager>::Get()->InterpolateGroupVolume(0.0f, comp->duration_FadeIn, "SFX");

	// Delete all bullets in the scene when player dies
    for (auto bullet = ecs::GetCompsActiveBegin<BulletComponent>(); bullet != ecs::GetCompsEnd<BulletComponent>(); ++bullet)
    {
        bullet->SetLifeTime(0.0f);
    }

    // Unsubscribe messages
    Messaging::Unsubscribe("DiedPlayer", OnDeath);
    Messaging::Unsubscribe("GameWin", OnGameWin);
}

// Called once when boss dies and all wave enemies dies
void GameOverSystem::OnGameWin()
{
    // Get component
    auto comp = ecs::GetCompsBegin<GameOverComponent>();
    if (comp == ecs::GetCompsEnd<GameOverComponent>())
    {
        CONSOLE_LOG(LEVEL_WARNING) << "Win condition triggered, but no GameOverComponent in scene!";
        return;
    }

    // Get entity
    ecs::EntityHandle entity = comp.GetEntity();

    // Sound management
    ST<AudioManager>::Get()->StopAllSounds();
    ST<AudioManager>::Get()->StartSingleSound("GameWin");

	// Set the game state to victory
    comp->SetPlayState(PLAYSTATE::END_ANIM_WIN_1);
    comp->text_GameStatus->GetTransform().SetLocalPosition(comp->text_GameStatus_DefaultLocalPosition);
    auto textComp = comp->text_GameStatus->GetComp<TextComponent>();
    textComp->SetText("You Win!");
    textComp->SetColor({ 0.0f, 1.0f, 1.0f, 1.0f });

    // Do END_ANIM_WIN_1
    Vector2 pos_text_Spacebar = comp->text_Spacebar->GetTransform().GetLocalPosition();
    Vector2 pos_text_GameStatus = comp->text_GameStatus->GetTransform().GetLocalPosition();

    // Tween space to return to title
    ST<TweenManager>::Get()->StartTween(
        comp->text_Spacebar,
        &Transform::SetLocalPosition,
        pos_text_Spacebar,
        Vector2{ 0.0f, pos_text_Spacebar.y },
        comp->duration_FadeIn,
        TT::EASE_BOTH);

    // Tween game over status (died or win)
    ST<TweenManager>::Get()->StartTween(
        comp->text_GameStatus,
        &Transform::SetLocalPosition,
        pos_text_GameStatus,
        Vector2{ 0.0f, pos_text_GameStatus.y },
        comp->duration_FadeIn,
        TT::EASE_BOTH);

    // Fade in to black
    ST<TweenManager>::Get()->StartTween(
        entity,
        &RenderComponent::SetColor,
        Vector4{ 0.0f, 0.0f, 0.0f, 0.0f },
        Vector4{ 0.0f, 0.0f, 0.0f, 1.0f },
        comp->duration_FadeIn,
        TT::EASE_IN);

    // Fade out the SFX
    ST<AudioManager>::Get()->InterpolateGroupVolume(0.0f, comp->duration_FadeIn, "SFX");

    // Unsubscribe messages
    Messaging::Unsubscribe("DiedPlayer", OnDeath);
    Messaging::Unsubscribe("GameWin", OnGameWin);

    // Update scoreboard
    auto scoreboard = ST<GameManager>::Get()->GetScoreboard();
    for (size_t i = 0; i < comp->vecScores.size(); ++i)
    {
        int currentScore = scoreboard[i].second;
        std::string scoreString = std::to_string(currentScore);
        Vector4 color{};

        auto multiRefVec = comp->vecScores[i]->GetComp<MultiReferenceComponent>()->GetReferences();

        // Special case for total score
        if (i == comp->vecScores.size() - 1)
        {
            currentScore = 0;
            multiRefVec[1]->GetComp<TextComponent>()->SetText("");
            multiRefVec[2]->GetComp<TextComponent>()->SetColor({ 1.0f, 1.0f, 0.0f, 0.0f });
        }
        else
        {
            // Different visuals based on positive or negative score
            if (currentScore > 0)
            {
                color = Vector4{ 0.0f, 1.0f, 0.0f, 0.0f };
                scoreString = "+" + scoreString;
            }
            else
            {
                color = Vector4{ 1.0f, 0.2f, 0.2f, 0.0f };
            }

            multiRefVec[1]->GetComp<TextComponent>()->SetText(std::to_string(scoreboard[i].first));
            multiRefVec[2]->GetComp<TextComponent>()->SetColor(color);
        }
        multiRefVec[2]->GetComp<TextComponent>()->SetText(scoreString);
    }
}
