/******************************************************************************/
/*!
\file   PersistentInteractableComponent.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   04/02/2025

\author Chan Kuan Fu Ryan (100%)
\par    email: c.kuanfuryan\@digipen.edu
\par    DigiPen login: c.kuanfuryan

\brief
	PersistentInteractableComponent is an ECS component inheriting from Highlightable
	class, identifying a scene entity as a persistent interactable object, such
	as a lever or button.

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#include "PersistentInteractableComponent.h"
#include "AudioManager.h"
#include "MultiSpriteComponent.h"
#include "EntityLayers.h"

PersistentInteractableComponent::PersistentInteractableComponent() :
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
	type{ 0 },
	activated{ false },
	audioUse{ "Lever" },
	Highlightable("Lever",7.0f,{KEY::F})
{
}

void PersistentInteractableComponent::OnUseStart(KEY use)
{
	switch (use)
	{
	case KEY::F:
	{
		ecs::EntityHandle entity = ecs::GetEntity(this);
		ecs::CompHandle<MultiSpriteComponent> multiSprite = entity->GetCompInChildren<MultiSpriteComponent>();

		// If it's single-use, we just change the multi-sprite and layer here
		if(type == INTERACTABLE_TYPE::SINGLE_USE)
		{
			CONSOLE_LOG(LogLevel::LEVEL_DEBUG) << "SINGLE USE CLICKED";
			entity->GetComp<EntityLayerComponent>()->SetLayer(ENTITY_LAYER::DEFAULT);
			activated = true;
			multiSprite->ChangeSprite(2);
		}
		else
		{
			activated = !activated;
			// 1 for unactivated, 3 for activated
			multiSprite->ChangeSprite(activated ? 3 : 1);
		}

		ST<AudioManager>::Get()->StartSound(audioUse);
		if (connectedEntity)
		{
			if (auto eventsComp{ connectedEntity->GetComp <EntityEventsComponent>() })
			{
				eventsComp->BroadcastAll("OnActivated");
			}
		}
		break;
	}
	}
}

void PersistentInteractableComponent::Serialize(Serializer& writer) const
{
	writer.Serialize("type", +type);
	writer.Serialize("audioUse", audioUse);
	writer.Serialize("connectedEntity", connectedEntity);
}

void PersistentInteractableComponent::Deserialize(Deserializer& reader)
{
	reader.DeserializeVar("type", reinterpret_cast<size_t*>(&type));
	reader.Deserialize("audioUse", &audioUse);
	reader.Deserialize("connectedEntity", &connectedEntity);
}

void PersistentInteractableComponent::SetIsHighlighted(bool highlighted)
{
		EntityReference entity = ecs::GetEntity(this);
	if (type == INTERACTABLE_TYPE::TOGGLEABLE)
	{
		ecs::CompHandle<MultiSpriteComponent> multiSprite = entity->GetCompInChildren<MultiSpriteComponent>();
		if (highlighted)
		{
			// 1 for unactivated, 3 for activated
			multiSprite->ChangeSprite(activated ?3:1);

		}
		else
		{
			// 0 for unactivated, 2 for activated
			multiSprite->ChangeSprite(activated ?2:0);
		}
	}
	else
	{
		if (activated) return;
		Highlightable::SetIsHighlighted(highlighted, entity);
	}
}

#ifdef IMGUI_ENABLED
void PersistentInteractableComponent::EditorDraw(PersistentInteractableComponent& comp)
{
	char const* interactableTypeNames[] = {
		"Single Use",
		"Toggleable",
		"Lever 2",
		"Secret Button"
	};
	int current = static_cast<int>(comp.type);

	if (ImGui::Combo("Interactable Type", &current, interactableTypeNames, IM_ARRAYSIZE(interactableTypeNames)))
	{
		comp.type = static_cast<INTERACTABLE_TYPE>(current);
	}
	comp.name.resize(16);
	ImGui::InputText("Name", comp.name.data(), 16);
	comp.audioUse.EditorDraw("Use Sound");
	ImGui::Checkbox("Activated", &comp.activated);
	comp.connectedEntity.EditorDraw("Connected Entity");
}
#endif
