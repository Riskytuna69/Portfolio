#include "ItemInfoPanel.h"
#include "TextComponent.h"
#include "Healthpack.h"
#include "Weapon.h"

ItemInfoPanelComponent::ItemInfoPanelComponent() :
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
	entityName{ nullptr },
	entityCurrent{ nullptr },
	entityMaximum{ nullptr }
{
}

void ItemInfoPanelComponent::UpdateItemInfo(EntityReference& item)
{
	// Initialise values
	std::string str_name{ "None" };
	std::string str_current{ "-" };
	std::string str_maximum{ "-" };

	// Get text components
	ecs::CompHandle<TextComponent> text_name = entityName->GetComp<TextComponent>();
	ecs::CompHandle<TextComponent> text_current = entityCurrent->GetComp<TextComponent>();
	ecs::CompHandle<TextComponent> text_maximum = entityMaximum->GetComp<TextComponent>();

	// If unable to get, post error
	if (text_name == nullptr || text_current == nullptr || text_maximum == nullptr)
	{
		CONSOLE_LOG(LogLevel::LEVEL_ERROR) << "ItemInfoPanel does not have 3 valid TextComponents!: ";
		return;
	}

	// Item must exist
	if (item)
	{
		// If weapon
		ecs::CompHandle<WeaponComponent> heldWeapon = item->GetComp<WeaponComponent>();
		if (heldWeapon)
		{
			str_name = heldWeapon->name;
			str_current = std::to_string(heldWeapon->magazineCurr);
			str_maximum = std::to_string(heldWeapon->stats.magazineSize);
		}
		else // else check if healthpack
		{
			ecs::CompHandle<HealthpackComponent> heldHealthpack = item->GetComp<HealthpackComponent>();
			if (heldHealthpack)
			{
				str_name = heldHealthpack->name;
			}
		}
	}
	text_name->SetText(str_name);
	text_current->SetText(str_current);
	text_maximum->SetText(str_maximum);
	return;
	
}

#ifdef IMGUI_ENABLED
void ItemInfoPanelComponent::EditorDraw(ItemInfoPanelComponent& comp)
{
	comp.entityName.EditorDraw("Text Name");
	comp.entityCurrent.EditorDraw("Text Current");
	comp.entityMaximum.EditorDraw("Text Maximum");
}
#endif
