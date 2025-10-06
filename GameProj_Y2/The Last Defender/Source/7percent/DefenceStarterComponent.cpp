/******************************************************************************/
/*!
\file   DefenceStarterComponent.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   01/24/2025

\author Marc Alviz Evangelista (100%)
\par    email: marcalviz.e\@digipen.edu
\par    DigiPen login: marcalviz.e

\brief

	Component responsible for starting the defence half of the game.


All content � 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#include "DefenceStarterComponent.h"
#include "Physics.h"
#include "Collision.h"
#include "EntityLayers.h"
#include "RenderComponent.h"
#include "ResourceManager.h"
#include "Player.h"
#include "GameManager.h"
#include "MultiSpriteComponent.h"

void DefenceStarterComponent::OnAttached()
{
	Messaging::Subscribe("GAMESTATEGRACE",  &DefenceStarterComponent::DisableAllButtons);

}

void DefenceStarterComponent::OnDetached()
{
	if (auto eventsComp{ ecs::GetEntity(this)->GetComp<EntityEventsComponent>() })
		Messaging::Unsubscribe("GAMESTATEGRACE", &DefenceStarterComponent::DisableAllButtons);

}

void DefenceStarterComponent::DisableAllButtons()
{
	for(auto button = ecs::GetCompsBegin< DefenceStarterComponent>();button!= ecs::GetCompsEnd< DefenceStarterComponent>();++button)
	{
		button->isActivated = true;
		ecs::EntityHandle entity = button.GetEntity();
		ecs::CompHandle<MultiSpriteComponent> multiSprite = entity->GetCompInChildren<MultiSpriteComponent>();
		multiSprite->ChangeSprite(2);

		entity->GetComp<EntityLayerComponent>()->SetLayer(ENTITY_LAYER::DEFAULT);
	}
}

DefenceStarterComponent::DefenceStarterComponent() :
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
	audioUse{ "Lever" },
	connectedEntity{ nullptr },
	isActivated{ false },
	Highlightable("Activate", 3.0f, {KEY::F})
{
}

DefenceStarterComponent::~DefenceStarterComponent()
{
}

void DefenceStarterComponent::Serialize(Serializer& writer) const
{
	writer.Serialize("audioUse", audioUse);
	writer.Serialize("connectedEntity", connectedEntity);
}

void DefenceStarterComponent::Deserialize(Deserializer& reader)
{
	reader.Deserialize("audioUse", &audioUse);
	reader.Deserialize("connectedEntity", &connectedEntity);
}

void DefenceStarterComponent::SetIsHighlighted(bool highlighted)
{
	if (isActivated) return;

	Highlightable::SetIsHighlighted(highlighted, ecs::GetEntity(this));

}

void DefenceStarterComponent::OnUseStart(KEY use)
{
	switch (use)
	{
	case KEY::F:
	{
		if (!isActivated)
		{
			isActivated = true;
			ST<GameManager>::Get()->SetGameState(GAME_STATE::GRACE);

			EntityReference entity = ecs::GetEntity(this);
			entity->GetComp<EntityLayerComponent>()->SetLayer(ENTITY_LAYER::DEFAULT);
			ecs::CompHandle<MultiSpriteComponent> multiSprite = entity->GetCompInChildren<MultiSpriteComponent>();
			multiSprite->ChangeSprite(2);
			ST<AudioManager>::Get()->StartSound(audioUse);
			Messaging::BroadcastAll("WaveStarted");

			if (connectedEntity)
			{
				if (auto eventsComp{ connectedEntity->GetComp <EntityEventsComponent>() })
				{
					eventsComp->BroadcastAll("OnActivated");
				}
			}
		}
		break;
	}
	}
}

#ifdef IMGUI_ENABLED
	void DefenceStarterComponent::EditorDraw(DefenceStarterComponent& comp)
	{
		comp.audioUse.EditorDraw("Use Sound");
		comp.connectedEntity.EditorDraw("Connected Door");
		ImGui::Checkbox("Activated", &comp.isActivated);
	}
#endif