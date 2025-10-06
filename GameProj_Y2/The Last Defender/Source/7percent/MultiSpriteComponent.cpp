/******************************************************************************/
/*!
\file   MultiSpriteComponent.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   04/02/2025

\author Chan Kuan Fu Ryan (100%)
\par    email: c.kuanfuryan\@digipen.edu
\par    DigiPen login: c.kuanfuryan

\brief
	MultiSpriteComponent is an ECS component that serializes a varied number of
	spriteIDs to be used for entites that need to render as different textures
	in a convenient manner.

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#include "MultiSpriteComponent.h"
#include "RenderComponent.h"

void MultiSpriteComponent::ChangeSprite(int spriteIndex)
{
	ecs::EntityHandle thisEntity { ecs::GetEntity(this) };
	ecs::CompHandle<RenderComponent> renderComp = thisEntity->GetComp<RenderComponent>();

	if (renderComp == nullptr)
	{
		CONSOLE_LOG_EXPLICIT("MultiSpriteComponent requires RenderComponent!", LogLevel::LEVEL_WARNING);
		return;
	}

	size_t spriteID = spriteID_Vec[spriteIndex];
	auto& sprite = ResourceManager::GetSprite(spriteID);

	// Set render component to show the new sprite
	renderComp->SetSpriteID(spriteID);

	// Resize scale transform to match
	thisEntity->GetTransform().SetLocalScale({ static_cast<float>(sprite.width), static_cast<float>(sprite.height) });
}

void MultiSpriteComponent::ChangeSpriteID(int spriteID)
{
	ecs::EntityHandle thisEntity{ ecs::GetEntity(this) };
	ecs::CompHandle<RenderComponent> renderComp = thisEntity->GetComp<RenderComponent>();

	// If negative sprite ID, set to transparent
	if (spriteID < 0)
	{
		renderComp->SetColor(Vector4(0.0f));
		return;
	}

	// Make sure render component is normal
	renderComp->SetColor(Vector4(1.0f));

	// Get sprite reference
	auto& sprite = ResourceManager::GetSprite(spriteID);

	// Set render component to show the new sprite
	renderComp->SetSpriteID(spriteID);

	// Resize scale transform to match
	thisEntity->GetTransform().SetLocalScale({ static_cast<float>(sprite.width), static_cast<float>(sprite.height) });
}

Sprite const& MultiSpriteComponent::GetSprite()
{
	size_t spriteID = spriteID_Vec[static_cast<int>(currentSpriteIndex)];
	return ResourceManager::GetSprite(spriteID);
}

MultiSpriteComponent::MultiSpriteComponent() :
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
	numSprites{ 2 },
	spriteID_Vec{ std::vector<size_t>(numSprites, 0) },
	currentSpriteIndex{ 0 }
{
}

#ifdef IMGUI_ENABLED
void MultiSpriteComponent::EditorDraw(MultiSpriteComponent& comp)
{
	// Input field to modify numSprites
	ImGui::InputInt("Number of Sprites", &comp.numSprites);

	// Clamp to 1
	if (comp.numSprites < 1)
	{
		comp.numSprites = 1;
	}

	// Add button to apply changes
	if (ImGui::Button("Resize"))
	{
		comp.spriteID_Vec.resize(static_cast<size_t>(comp.numSprites));
	}
	std::string indexText = "Index: "+std::to_string(comp.currentSpriteIndex);
	ImGui::Text(indexText.c_str());

	auto DrawSpriteInput = [](size_t& spriteID, const std::string& name)
	{
		if (!ResourceManager::SpriteExists(spriteID))
		{
			ImGui::Text("No Sprite Assigned, drag a sprite here to assign it");
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SPRITE_ID"))
				{
					spriteID = *static_cast<size_t*>(payload->Data);
				}
				ImGui::EndDragDropTarget();
			}
			return;
		}
		auto& sprite = ResourceManager::GetSprite(spriteID);
		ImGui::Text("Sprite Name: %s", sprite.name.c_str());

		ImGui::Text("Drag an Sprite from the browser to assign it");
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::ImageButton(name.c_str(), VulkanManager::Get().VkTextureManager().getTexture(sprite.textureID).ImGui_handle, ImVec2(100, 100),
			ImVec2(sprite.texCoords.x, sprite.texCoords.y),
			ImVec2(sprite.texCoords.z, sprite.texCoords.w));
		ImGui::PopItemFlag();

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SPRITE_ID"))
			{
				spriteID = *static_cast<size_t*>(payload->Data);
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::Text("Width: %d px, Height: %d px", sprite.width, sprite.height);
	};
	int s = 0;
	for (size_t& val : comp.spriteID_Vec)
	{
		DrawSpriteInput(val, std::to_string(s++));
	}
}
#endif