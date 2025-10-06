/******************************************************************************/
/*!
\file   Intel.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   09/03/2025

\author Chan Kuan Fu Ryan (10%)
\par    email: c.kuanfuryan\@digipen.edu
\par    DigiPen login: c.kuanfuryan

\author Matthew Chan Shao Jie (90%)
\par    email: m.chan\@digipen.edu
\par    DigiPen login: m.chan

\brief
  Collectible item. Each represents a short story relating to the game world.
  Identified through integral value, id.

All content © 2025 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#include "Intel.h"
#include "GameManager.h"
#include "TweenManager.h"
#include "PrefabManager.h"
#include "Player.h"
#include "TextComponent.h"
#include "PrefabManager.h"
#include "EnemyStateMachine.h"

IntelComponent::IntelComponent() :
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
	id{ 0 },
	audioUse{ "Intel_Pickup" },
	Highlightable("Intel", 0.0f, { KEY::F })
{
}

void IntelComponent::OnUseStart(KEY use)
{
	switch (use)
	{
	case KEY::F:
	{
		ST<GameManager>::Get()->SetIntelUnlocked(id, true);
		ST<AudioManager>::Get()->StartSound(audioUse, false, std::nullopt, 2.0f);

		// Get player reference
		ecs::CompHandle<PlayerComponent> playerComp = ecs::GetCompsBegin<PlayerComponent>().GetComp();

		// Load a text prefab and assign text
		ecs::EntityHandle textEntity = ST<PrefabManager>::Get()->LoadPrefab("IntelText");
		ecs::CompHandle<TextComponent> textComp = textEntity->GetComp<TextComponent>();
		std::ostringstream message ("");
		int totalCollected = ST<GameManager>::Get()->GetTotalIntelUnlocked();
		int totalToCollect = ST<GameManager>::Get()->GetTotalIntel();

		// Set text parent to player
		textEntity->GetTransform().SetParent(ecs::GetEntityTransform(playerComp));
		textEntity->GetTransform().SetLocalPosition({ 0.0f,0.5f });
		Vector4 startColor = Vector4{ 1.0f };
		Vector4 endColor = Vector4{ 1.0f };
		endColor.w = 0.0f;

		float transitionTime = 1.5f;

		if (totalToCollect == totalCollected)
		{
			// Activate the Noisy Cricket Easter Egg
			//Vector2 hardCodedSpawnLocation{ -839.0f,-1355.0f };
			//ecs::EntityHandle enemyEntity = ST<PrefabManager>::Get()->LoadPrefab("PatrolEnemy");
			//enemyEntity->GetTransform().SetWorldPosition(hardCodedSpawnLocation);
			//ecs::CompHandle<EnemyControllerComponent> enemyComp = enemyEntity->GetComp< EnemyControllerComponent>();
			//enemyComp->weaponPrefabName = "Weapon_NoisyCricket";
			message << "All intel collected! A door has opened...";
			transitionTime = 10.0f;
		}
		else
		{
			message << totalCollected  << "/"<< totalToCollect << " COLLECTED";
		}

		textComp->SetText(message.str());
		// Assign Tween
		ST<TweenManager>::Get()->StartTween(
			textEntity,
			&TextComponent::SetColor,
			startColor,
			endColor,
			transitionTime,
			TT::LINEAR);
		ST<TweenManager>::Get()->StartTween(
			textEntity,
			&Transform::SetLocalPosition,
			textEntity->GetTransform().GetLocalPosition(),
			Vector2{ 0.0f,1.0f },
			transitionTime,
			TT::LINEAR);

		// Schedule extermination
		ST<Scheduler>::Get()->Add(transitionTime, [textEntity]
			{
				// Move the camera
				if (!textEntity) return;
				ecs::DeleteEntity(textEntity);
			});
		
		// Delete this entity
		ecs::DeleteEntity(ecs::GetEntity(this));
		break;
	}
	}
}

void IntelComponent::SetIsHighlighted(bool highlighted)
{
	Highlightable::SetIsHighlighted(highlighted, ecs::GetEntity(this));
}

#ifdef IMGUI_ENABLED
void IntelComponent::EditorDraw(IntelComponent& comp)
{
	ImGui::InputInt("ID", &comp.id);
}
#endif