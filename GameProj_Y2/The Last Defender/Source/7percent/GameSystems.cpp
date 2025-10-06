/******************************************************************************/
/*!
\file   GameSystems.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   11/02/2024

\author Kendrick Sim Hean Guan (100%)
\par    email: kendrickheanguan.s\@digipen.edu
\par    DigiPen login: kendrickheanguan.s

\brief
  This is the source file implementing state classes that manage which systems are
  loaded into ECS.

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/

#include "GameSystems.h"
#include "ECSSysLayers.h"

#include "AnimatorSystem.h"
#include "ArmPivotIK.h"
#include "RenderSystem.h"
#include "TextSystem.h"
#include "ScriptComponent.h"
#include "Parallax.h"
#include "CheckBoxComponent.h"
#include "Slider.h"

#include "Character.h"
#include "ObjectiveTimer.h"
#include "Player.h"
#include "Weapon.h"
#include "Health.h"
#include "Button.h"
#include "CheatCodes.h"
#include "CameraSystem.h"
#include "Physics.h"
#include "Collision.h"
#include "TweenECS.h"
#include "Bullet.h"
#include "EnemyStateMachine.h"
#include "EnemySpawnSystem.h"
#include "EnemyBomb.h"
#include "MainObjective.h"
#include "GameOver.h"
#include "AudioListener.h"
#include "LightingSystem.h"
#include "JumpPad.h"
#include "PauseSystem.h"
#include "GameCameraController.h"
#include "Shield.h"
#include "EnemySpawnSystem.h"
#include "DefenceStarterComponent.h"
#include "SceneSoundManager.h"
#include "MenuSoundManager.h"
#include "AudioSystem.h"
#include "BossAIComponent.h"
#include "InventoryContainer.h"
#include "ReticleSpread.h"
#include "ObjectiveArrow.h"
#include "IntelViewer.h"
#include "HitFlash.h"
#include "Rotater.h"
#include "TutorialSoundManager.h"
#include "Confirmation.h"
#include "TrailSystem.h"
#include "KillAnimationWhenFinish.h"
#include "GamepadInputAdapter.h"
#include "CeilingTurret.h"
#include "FadeAndDie.h"
#include "Checkpoint.h"
#include "CutsceneManager.h"
#include "PrefabSpawner.h"
#include "SplashComponent.h"

void GameStateBase::OnExit()
{
	ecs::RemoveAllSystems();
}

void GameState_Common::OnEnter()
{
    ecs::AddSystem(ECS_LAYER::RENDER_0, RenderSystem{});
    ecs::AddSystem(ECS_LAYER::RENDER_0, TrailRendererDrawingSystem{});
    ecs::AddSystem(ECS_LAYER::RENDER_0, LightingSystem{});
    ecs::AddSystem(ECS_LAYER::RENDER_0, PostProcessingSystem{});
    ecs::AddSystem(ECS_LAYER::RENDER_1, Physics::ColliderBorderSystem{});
    ecs::AddSystem(ECS_LAYER::RENDER_1, Physics::PhysicsVelocityDebugSystem{});
    ecs::AddSystem(ECS_LAYER::RENDER_UI_0, TextSystem{});
    ecs::AddSystem(ECS_LAYER::PRE_PHYSICS_0, FPSTextSystem{});
}

void GameState_Editor::OnEnter()
{
    GameState_Common::OnEnter();

    ecs::AddSystem(ECS_LAYER::PRE_PHYSICS_0, AnimatorSystem{});
    ecs::AddSystem(ECS_LAYER::PRE_PHYSICS_0, TrailRendererSystem{});
    ecs::AddSystem(ECS_LAYER::PRE_UPDATE_0, UndoShakeSystem{});
    ecs::AddSystem(ECS_LAYER::POST_PHYSICS_1, ShakeSystem{});

    //ecs::AddSystem(ECS_LAYER::POST_PHYSICS_0, ArmPivotIKSystem{}); // CHANGE LATER
}

void GameState_Game::OnEnter()
{
    GameState_Common::OnEnter();

    ecs::AddSystem(ECS_LAYER::PRE_UPDATE_0, UndoShakeSystem{});

    ecs::AddSystem(ECS_LAYER::REALTIME_INPUT_0, ButtonSystem{});
    ecs::AddSystem(ECS_LAYER::REALTIME_INPUT_0, PauseSystem{});
    ecs::AddSystem(ECS_LAYER::REALTIME_INPUT_0, CheckBoxSystem{});
    ecs::AddSystem(ECS_LAYER::REALTIME_INPUT_0, SliderSystem{});
    ecs::AddSystem(ECS_LAYER::REALTIME_INPUT_0, IntelViewerSystem{});
    ecs::AddSystem(ECS_LAYER::REALTIME_INPUT_0, ConfirmationSystem{});
    ecs::AddSystem(ECS_LAYER::REALTIME_INPUT_0, GamepadAimAdapterSystem{});
    ecs::AddSystem(ECS_LAYER::REALTIME_INPUT_0, GamepadMouseControlSystem{});
    
    ecs::AddSystem(ECS_LAYER::PRE_PHYSICS_0, AnimatorSystem{});
    ecs::AddSystem(ECS_LAYER::PRE_PHYSICS_0, TrailRendererSystem{});
    ecs::AddSystem(ECS_LAYER::PRE_PHYSICS_0, LightBlinkSystem{});
    ecs::AddSystem(ECS_LAYER::PRE_PHYSICS_0, ObjectiveTimerSystem{});
    ecs::AddSystem(ECS_LAYER::SCRIPT_PREAWAKE, ScriptPreAwakeSystem{});
    ecs::AddSystem(ECS_LAYER::SCRIPT_AWAKE, ScriptAwakeSystem{});
    ecs::AddSystem(ECS_LAYER::SCRIPT_START, ScriptStartSystem{});
    ecs::AddSystem(ECS_LAYER::SCRIPT_UPDATE, ScriptSystem{});
    ecs::AddSystem(ECS_LAYER::INPUT_0, WeaponSystem{});
    ecs::AddSystem(ECS_LAYER::INPUT_0, CharacterSystem{});
    ecs::AddSystem(ECS_LAYER::INPUT_0, CheatCodes{});
    ecs::AddSystem(ECS_LAYER::INPUT_0, ShieldInputSystem{});
    ecs::AddSystem(ECS_LAYER::PHYSICS, Physics::PhysicsSystem{});
    ecs::AddSystem(ECS_LAYER::COLLISION, ShieldSystem{});
    ecs::AddSystem(ECS_LAYER::COLLISION, GameCameraControllerSystem{});
    ecs::AddSystem(ECS_LAYER::COLLISION, Physics::CollisionSystem{});
    ecs::AddSystem(ECS_LAYER::COLLISION, JumpPadSystem{});
    ecs::AddSystem(ECS_LAYER::COLLISION, EnemyBombSystem{});
    ecs::AddSystem(ECS_LAYER::POST_PHYSICS_1, HealthSystem{});
    ecs::AddSystem(ECS_LAYER::POST_PHYSICS_2, HealthVignetteSystem{});
    ecs::AddSystem(ECS_LAYER::POST_PHYSICS_1, CeilingTurretSystem{});
    ecs::AddSystem(ECS_LAYER::POST_PHYSICS_0, ArmPivotIKSystem{});
    ecs::AddSystem(ECS_LAYER::POST_PHYSICS_0, HealthRegenSystem{});
    ecs::AddSystem(ECS_LAYER::POST_PHYSICS_0, CutsceneSystem{});
    ecs::AddSystem(ECS_LAYER::TWEENING, TweenSystem{});
    ecs::AddSystem(ECS_LAYER::AUDIO, AudioListenerSystem{});
    ecs::AddSystem(ECS_LAYER::AUDIO, AudioSystem{});
    ecs::AddSystem(ECS_LAYER::AUDIO, SceneSoundManagerSystem{});
    ecs::AddSystem(ECS_LAYER::AUDIO, MenuSoundManagerSystem{});
    ecs::AddSystem(ECS_LAYER::AUDIO, TutorialSoundManagerSystem{});

    ecs::AddSystem(ECS_LAYER::PRE_PHYSICS_0, SplashSystem{});
    ecs::AddSystem(ECS_LAYER::POST_PHYSICS_2, CameraSystem{});
    ecs::AddSystem(ECS_LAYER::POST_PHYSICS_3, ParallaxSystem{});
    ecs::AddSystem(ECS_LAYER::POST_PHYSICS_0, AnchorToCameraSystem{});
    ecs::AddSystem(ECS_LAYER::POST_PHYSICS_0, AnimationFinishSystem{});
    ecs::AddSystem(ECS_LAYER::POST_PHYSICS_1, ShakeSystem{});
    ecs::AddSystem(ECS_LAYER::POST_PHYSICS_1, InventoryContainerReddenSystem{});
    ecs::AddSystem(ECS_LAYER::POST_PHYSICS_1, ReticleSpreadSystem{});
    ecs::AddSystem(ECS_LAYER::SCRIPT_LATE_UPDATE, ScriptLateUpdateSystem{});

    ecs::AddSystem(ECS_LAYER::PRE_PHYSICS_0, PlayerSystem{});
    ecs::AddSystem(ECS_LAYER::PRE_PHYSICS_0, EnemySpawnSystem{});
    ecs::AddSystem(ECS_LAYER::PRE_PHYSICS_0, BulletMovementSystem{});
    ecs::AddSystem(ECS_LAYER::PRE_PHYSICS_0, LaserRendererSystem{});
    ecs::AddSystem(ECS_LAYER::PRE_PHYSICS_0, BossLaserRendererSystem{});
    ecs::AddSystem(ECS_LAYER::PRE_PHYSICS_0, GameOverSystem{});
    ecs::AddSystem(ECS_LAYER::PRE_PHYSICS_0, EnemyControllerSystem{});
    ecs::AddSystem(ECS_LAYER::PRE_PHYSICS_0, BossAISystem{});
    ecs::AddSystem(ECS_LAYER::PRE_PHYSICS_0, ObjectiveArrowSystem{});
    ecs::AddSystem(ECS_LAYER::PRE_PHYSICS_0, HitFlashSystem{});
    ecs::AddSystem(ECS_LAYER::PRE_PHYSICS_0, RotaterSystem{});
    ecs::AddSystem(ECS_LAYER::PRE_PHYSICS_0, CheckpointSystem{});
    ecs::AddSystem(ECS_LAYER::POST_PHYSICS_0, FadeAndDieSystem{});
    ecs::AddSystem(ECS_LAYER::POST_PHYSICS_0, PrefabSpawnSystem{});

    
    ecs::AddSystem(ECS_LAYER::RENDER_1, Physics::QuadtreeRenderSystem{});
}

void GameState_Pause::OnEnter()
{
    GameState_Common::OnEnter();
}

void GameStateManager::Exit()
{
    SimpleStateMachine::SwitchToState(nullptr);
}
