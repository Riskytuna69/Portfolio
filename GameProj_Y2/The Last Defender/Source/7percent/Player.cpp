/******************************************************************************/
/*!
\file   Player.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   01/31/2025

\author Matthew Chan Shao Jie (100%)
\par    email: m.chan\@digipen.edu
\par    DigiPen login: m.chan

\brief
  This file handles all the logic for the Player Component.

All content � 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#include "ComponentLookupWorkaround.h"
#include "NameComponent.h"
#include "Healthpack.h"
#include "Player.h"
#include "Physics.h"
#include "Collision.h"
#include "Shield.h"
#include "Weapon.h"
#include "GameManager.h"
#include "PersistentInteractableComponent.h"
#include "Bar.h"
#include "InventoryUIManager.h"
#include "MultiSpriteComponent.h"
#include "AudioManager.h"
#include "TextComponent.h"
#include "ItemInfoPanel.h"
#include "Intel.h"
#include "HiddenSwitch.h"
#include "DefenceStarterComponent.h"

Holdable* GetHoldableFromEntity(ecs::EntityHandle entity)
{
	Holdable* hComp = nullptr;
	if ((hComp = entity->GetComp<WeaponComponent>()) != nullptr)
		return hComp;
	else if ((hComp = entity->GetComp<HealthpackComponent>()) != nullptr)
		return hComp;
	return hComp;
}
Highlightable* GetHighlightableFromEntity(ecs::EntityHandle entity)
{
	Highlightable* hComp = nullptr;
	if ((hComp = entity->GetComp<PersistentInteractableComponent>()) != nullptr)
		return hComp;
	else if ((hComp = entity->GetComp<IntelComponent>()) != nullptr)
		return hComp;
	else if ((hComp = entity->GetComp<SnoipahSpawner>()) != nullptr)
 		return hComp;
	else if ((hComp = entity->GetComp<DefenceStarterComponent>()) != nullptr)
 		return hComp;
	return GetHoldableFromEntity(entity);
}


void PlayerComponent::Init()
{
	inited = true;

	thisEntity = ecs::GetEntity(this);
	armPivot = ecs::GetEntity(thisEntity->GetCompInChildren<ArmPivot>());

	ecs::CompHandle<InventoryUIManagerComponent> invManager = inventoryUIComponent->GetComp<InventoryUIManagerComponent>();
	invManager->Initialise();
	UpdateHeldItems();

	// Reset all game vars
	ST<GameManager>::Get()->ResetGameVariables();

	// Broadcast restart
	Messaging::BroadcastAll("GAMERESTART");

	// Reset states
	ST<Game>::Get()->ResetState();
	ST<GameManager>::Get()->SetGameState(GAME_STATE::CRAWLING);

	// Register OnCollision to Weapon Grabber
	if (weaponGrabber)
		weaponGrabber->GetComp<EntityEventsComponent>()->Subscribe("OnCollision", this, &PlayerComponent::OnCollision);
}

int PlayerComponent::PickUpItem(ecs::CompHandle<Holdable> item, ecs::EntityHandle itemEntity)
{
	int slot{ -1 };

	// Attempt to pick up to the first free slot
	for (int i{ currentHoldingItem }; i < unlockedSlots; ++i)
	{
		if (!heldItems[i])
		{
			slot = i;
			heldItems[i] = itemEntity;
			break;
		}
	}
	// If there were no free slots, throw the current item and replace.
	if (slot < 0)
	{
		ThrowCurrentItem();
		slot = currentHoldingItem;
		CurrentItem() = itemEntity;
	}

	// Housekeeping when an inventory slot changes.
	UpdateHeldItems();
	ST<AudioManager>::Get()->StartSound(audioHolster);

	// If weapon, also play unique pickup sound
	if (ecs::CompHandle<WeaponComponent> weaponComp{ dynamic_cast<ecs::CompHandle<WeaponComponent>>(item) })
	{
		weaponComp->isPlayerWeapon = true;
		ST<AudioManager>::Get()->StartSound(weaponComp->audioPickup);
		ST<GameManager>::Get()->OnWeaponFound(weaponComp->name);
	}

	return slot;
}

void PlayerComponent::ThrowCurrentItem()
{
	// Verify existence
	if (!heldItems[currentHoldingItem].IsValidReference())
		return;

	// Get this entity
	ecs::EntityHandle currentItem = heldItems[currentHoldingItem];

	// Attempt to get either a weapon or healthpack
	ecs::CompHandle<Holdable> heldItem = nullptr;


	switch (Holdable::GetHoldable(currentItem, &heldItem))
	{
	case Holdable::HOLDABLE_TYPE::NONE:
		// Sanity check because everything else after this WILL crash
		return;
	}

	// Call OnDropped on the item
	heldItem->OnDropped();

	// Item is no longer held
	heldItem->isHeld = false;

	// Set layer back to WEAPON
	currentItem->GetComp<EntityLayerComponent>()->SetLayer(ENTITY_LAYER::WEAPON);

	// Launch the item forward and set it to dynamic
	if (ecs::CompHandle<Physics::PhysicsComp> physComp = currentItem->GetComp<Physics::PhysicsComp>())
	{
		physComp->SetFlag(Physics::PHYSICS_COMP_FLAG::IS_DYNAMIC, true);
		float scale = armPivot->GetTransform().GetWorldScale().x;
		float rotation = currentItem->GetTransform().GetWorldRotation();
		if (scale < 0)
			rotation = 180.0f - rotation;
		rotation = math::ToRadians(rotation);
		Vector2 throwVel{ cosf(rotation),sinf(rotation) };
		physComp->SetVelocity(throwVel * 800.0f);
	}

	// Disown the child
	currentItem->GetTransform().SetParent(nullptr);
	heldItems[currentHoldingItem] = nullptr;

	// Reset world rotation
	currentItem->GetTransform().SetWorldRotation(0.0f);

	// Update UI
	UpdateHeldItems();

	// Play audio
	ST<AudioManager>::Get()->StartSound(audioThrow);

	wasItemThrown = true;
}

PlayerComponent::PlayerComponent() :
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
	itemGrabDistance{ 100 },
	unlockedSlots{ 1 },
	itemSwitchCooldown{ 0.2f },

	reloadBar{ nullptr },
	heldItems{ nullptr,nullptr,nullptr,nullptr,nullptr },
	lastHighlightedEntity{ nullptr },
	armPivot{ nullptr },

	thisEntity{ nullptr },

	inited{ false },
	currentHoldingItem{ 0 },
	currentSwitchCooldown{ 0.0f },

	inventoryUIComponent{ nullptr },
	itemInfoPanelEntity{ nullptr },

	audioThrow{ "Throw" },
	audioHolster{ "Holster" },
	audioInvalid{ "Invalid" },
	wasItemInteractedWith{ false },
	wasItemSwitched{ false },
	wasItemThrown{ false }
{
}

void PlayerComponent::PackItemsLeft()
{
	int numSlotsToMove{};
	for (int i{}; i < unlockedSlots; ++i)
	{
		// Count empty slots
		if (!heldItems[i])
		{
			++numSlotsToMove;
			continue;
		}

		// There's an item here. Check if we need to move it
		if (!numSlotsToMove)
			continue;

		// We need to move it.
		heldItems[i - numSlotsToMove] = heldItems[i];
		heldItems[i] = nullptr;

		// If we selected this item, move the selection so we stay on the item's new position
		if (currentHoldingItem == i)
			currentHoldingItem -= numSlotsToMove;
	}
	
	// Block all hold states
	BlockUseKeys();

	// If we have no selection, force selection to the last item
	if (!heldItems[currentHoldingItem])
	{
		for (int i{ currentHoldingItem - 1 }; i >= 0; --i)
			if (heldItems[i])
			{
				currentHoldingItem = i;

				// Call switched to here
				CurrentHoldableSwitch(true);
				break;
			}
		// If there are no items in inventory, select the first slot.
		if (!heldItems[currentHoldingItem])
			currentHoldingItem = 0;
	}
	// Update the shown sprites
	UpdateHeldItems();
}

void PlayerComponent::BlockUseKeys()
{
	Holdable* hComp = CurrentHoldable();

	for (auto& pair : holdState)
	{
		if (pair.second == 1)
		{
			if (hComp)
			{
				hComp->OnUseEnd(pair.first);
			}
			pair.second = -1;
		}
	}
}

void PlayerComponent::OnAttached()
{
}

void PlayerComponent::OnDetached()
{
	if(weaponGrabber)
	{
		if (auto eventsComp{ weaponGrabber->GetComp<EntityEventsComponent>() })
			eventsComp->Unsubscribe("OnCollision", this, &PlayerComponent::OnCollision);
	}
}

bool PlayerComponent::HasEmptySlots()
{
	for (int i = 0; i < unlockedSlots; ++i)
	{
		if (heldItems[i] == nullptr)
			return true;
	}
	return false;
}

void PlayerComponent::OnCollision(const Physics::CollisionEventData& collisionData)
{
	// Get the entity
	ecs::EntityHandle otherEntity = ecs::GetEntity(collisionData.otherComp);

	// Don't autopickup if slots are full
	if (HasEmptySlots())
	{

		// Sanity check
		if (Holdable * holdableComp{ GetHoldableFromEntity(otherEntity) })
		{
			// Check if the object can be picked up
			if (holdableComp->CanAutoPickup())
			{
				PickUpItem(holdableComp, otherEntity);
				//ChangeItem(slot);
				//AttemptItemPickup(comp, hitEntity);

			}
		}
	}
	// Handle Intel
	if (Highlightable * highlightableComp{ GetHighlightableFromEntity(otherEntity) })
	{
		if (IntelComponent * intelComp{ dynamic_cast<IntelComponent*>(highlightableComp) })
		{
			// Grab ittttt
			intelComp->OnUseStart(KEY::F);
		}
	}
}

ecs::CompHandle<Holdable> PlayerComponent::CurrentHoldable()
{
	Holdable* hComp = nullptr;
	if (CurrentItem())
	{
		if ((hComp = CurrentItem()->GetComp<WeaponComponent>())!=nullptr)
		{
		}
		else if ((hComp = CurrentItem()->GetComp<HealthpackComponent>())!=nullptr)
		{
		}
	}
	return hComp;
}

EntityReference& PlayerComponent::CurrentItem()
{
	if (currentHoldingItem >= unlockedSlots)
		currentHoldingItem = unlockedSlots;
	return heldItems[currentHoldingItem];
}

void PlayerComponent::ChangeItem(int item)
{
	wasItemSwitched = true;
	// Do nothing if we're switching to the same item lol
	if (currentHoldingItem == item)
		return;

	int targetingItem = math::Repeat(item, 0, unlockedSlots);
	bool playHolsterSound = true;
	if (heldItems[targetingItem] != nullptr)
	{
		CurrentHoldableSwitch(false);
		currentHoldingItem = targetingItem;
		CurrentHoldableSwitch(true);

		// Block all hold states
		BlockUseKeys();
	}
	else
	{
		//Inventory UI Manager flash here
		inventoryUIComponent->GetComp<InventoryUIManagerComponent>()->ShakeContainer(targetingItem);
		playHolsterSound = false;
	}

	// Update held item with a function
	UpdateHeldItems();

	// Play audio
	if (currentSwitchCooldown <= 0.0f)
	{
		if (playHolsterSound)
		{
			ST<AudioManager>::Get()->StartSound(audioHolster);
		}
		else
		{
			ST<AudioManager>::Get()->StartSingleSound(audioInvalid);
		}
		currentSwitchCooldown = itemSwitchCooldown;
	}

}

void PlayerComponent::CurrentHoldableSwitch(bool toThis)
{
	if (CurrentHoldable())
	{
		CurrentHoldable()->OnSwitched(toThis);
	}
}

void PlayerComponent::ChangeItem(bool next)
{
	// We iterate through all the slots to find one that is not empty
	int targetingItem = math::Repeat(currentHoldingItem + (next ? 1 : -1), 0, unlockedSlots);
	while (targetingItem != currentHoldingItem)
	{
		if (heldItems[targetingItem] != nullptr)
			break;
		targetingItem = math::Repeat(targetingItem + (next ? 1 : -1), 0, unlockedSlots);
	}

	// If we've selected the same slot as before, this means we can't scroll
	bool playHolsterSound = true;
	if (targetingItem == currentHoldingItem)
	{
		inventoryUIComponent->GetComp<InventoryUIManagerComponent>()->ShakeContainer(currentHoldingItem);
		playHolsterSound = false;
	}
	else
	{
		// Block all hold states
		BlockUseKeys();
		CurrentHoldableSwitch(false);
		currentHoldingItem = targetingItem;
		CurrentHoldableSwitch(false);
		UpdateHeldItems();
	}


	// Play audio
	if (currentSwitchCooldown <= 0.0f)
	{
		if (playHolsterSound)
		{
			ST<AudioManager>::Get()->StartSound(audioHolster);
		}
		else
		{
			ST<AudioManager>::Get()->StartSingleSound(audioInvalid);
		}
		currentSwitchCooldown = itemSwitchCooldown;
	}
}

void PlayerComponent::UpdateHeldItems()
{
	// Get the InventoryUIManagerComponent
	ecs::CompHandle< InventoryUIManagerComponent> invManager = inventoryUIComponent->GetComp<InventoryUIManagerComponent>();

	// Set unlocked containers
	invManager->SetNumberOfUnlockedContainers(unlockedSlots);

	// Update currently selected container
	invManager->SetCurrentlySelectedContainer(currentHoldingItem);

	//Pre-emptively hide rhe reload bar
	reloadBar->SetActive(false);

	// For each container
	for (int i = 0; i < unlockedSlots; ++i)
	{
		// Update the item sprites
		bool itemExists{ heldItems[i] };
		invManager->SetContainerDisplaySprite(static_cast<int>(itemExists ? heldItems[i]->GetCompInChildren<MultiSpriteComponent>()->spriteID_Vec[0] : -1), i);

		// Hide/Unhide the item that's in the world
		if (itemExists)
			heldItems[i]->SetActive(i == currentHoldingItem);
	}

	// Pre-emptively unload the reloadBar
	reloadBar->SetActive(false);

	// Update reload bar
	if (CurrentItem())
	{
		if (ecs::CompHandle<WeaponComponent> weapon{ CurrentItem()->GetComp<WeaponComponent>() })
		{
			reloadBar->SetActive(weapon->isReloading);
			reloadBar->GetCompInChildren<BarComponent>()->SetMax(weapon->stats.reloadTime);
		}
	}
}

#ifdef IMGUI_ENABLED
void PlayerComponent::EditorDraw(PlayerComponent& comp)
{
	ImGui::InputInt("Grab Range (Pixels): ", &comp.itemGrabDistance);
	comp.reloadBar.EditorDraw("Reload Bar");
	comp.inventoryUIComponent.EditorDraw("Inventory UI Manager");
	comp.pickupUIComponent.EditorDraw("Pickup UI");
	comp.itemInfoPanelEntity.EditorDraw("Info Panel UI");
	comp.weaponGrabber.EditorDraw("Weapon Grabber");
	ImGui::SliderInt("Unlocked slots", &comp.unlockedSlots, 1, 5);
	ImGui::InputFloat("Switch sound cooldown", &comp.itemSwitchCooldown);

	comp.audioThrow.EditorDraw("Throw Sound");
	comp.audioHolster.EditorDraw("Holster Sound");
	comp.audioInvalid.EditorDraw("Invalid Sound");
}
#endif

PlayerSystem::PlayerSystem()
	: System_Internal{ &PlayerSystem::UpdatePlayerComp }
{
}

void PlayerSystem::UpdatePlayerComp(PlayerComponent& comp)
{
	// Check if the comp is inited (for Inventory UI pruposes
	if (!comp.inited)
		comp.Init();

	// Skip if paused
	if (ST<GameManager>::Get()->GetPaused())
		return;

	// Raycast for items (we'll need this for highlighting later as well)
	ecs::EntityHandle hitEntity{ RaycastItem(comp) };

	// Check for item pickup input
	if (hitEntity)
	{
		if(Input::GetKeyCurr(KEY::F))
		{
			if(!comp.wasItemInteractedWith)
			AttemptItemPickup(comp, hitEntity);
		}
		else
		{
			comp.wasItemInteractedWith = false;
		}


		if (auto hComp = hitEntity->GetComp<SnoipahSpawner>())
		{
			for (auto key : hComp->useKeys)
			{
				if (comp.holdState[key] != -1)
				{
					if (Input::GetKeyPressed(key))
					{
						hComp->OnUseStart(key);
					}
					if (Input::GetKeyCurr(key))
					{
						hComp->OnUseHold(key);
					}
					else
					{
						hComp->OnUseEnd(key);
					}
				}
			}

		}
	}

	// Discarding current item
	if (Input::GetKeyCurr(KEY::B))
	{
		if(!comp.wasItemThrown)
		{
			comp.ThrowCurrentItem();
			comp.PackItemsLeft();
		}
	}
	else
		comp.wasItemThrown = false;

	// Item Switch handling
	if (Input::GetScroll() != 0.0f)
		comp.ChangeItem(Input::GetScroll() < 0.0f);

	//Item switch handling with number keys
	bool attemptingSwitch = false;
	for (int i = 0; i < comp.unlockedSlots; ++i)
	{
		if (Input::GetKeyCurr(KEY::NUM_1 + i))
		{
			attemptingSwitch = true;
			if(!comp.wasItemSwitched)
			comp.ChangeItem(i);
		}
	}
	if (!attemptingSwitch)
		comp.wasItemSwitched = false;

	// Lower item switch cooldown
	comp.currentSwitchCooldown -= GameTime::FixedDt();

	// Highlighting logic
	Highlightable* hComp = nullptr;
	comp.pickupUIComponent->SetActive(false);
	if (hitEntity != nullptr)
	{	
		hComp = GetHighlightableFromEntity(hitEntity);
		if (hComp)
		{
			if (hComp->interactable)
			{
				hComp->SetIsHighlighted(true);
				//CONSOLE_LOG(LogLevel::LEVEL_DEBUG) << "HIGHLIGHTED: " << hComp->name;
				comp.pickupUIComponent->SetActive(true);
				comp.pickupUIComponent->GetCompInChildren<TextComponent>()->SetText(hComp->name);
			}
		}
	}
	if (hitEntity != comp.lastHighlightedEntity)
	{
		if (comp.lastHighlightedEntity != nullptr)
		{
			hComp = GetHighlightableFromEntity(comp.lastHighlightedEntity);
		}
		if(hComp)
		{
			hComp->SetIsHighlighted(false);
		}
		comp.lastHighlightedEntity = hitEntity;
	}

	//Need the band-aid here, unless setting 
	if (hitEntity)
		comp.pickupUIComponent->GetTransform().SetWorldPosition(hitEntity->GetTransform().GetWorldPosition() + Vector2{0.0f,hComp->textHeightOffset});

	// Update info panel, per-frame.
	if (comp.itemInfoPanelEntity)
	{
		ecs::CompHandle<ItemInfoPanelComponent> infoPanel = comp.itemInfoPanelEntity->GetComp<ItemInfoPanelComponent>();
		if (infoPanel)
		{
			// We simply pass the currently held item into the panel.
			// It is up to the UI logic to decide what to do with the data.
			infoPanel->UpdateItemInfo(comp.CurrentItem());
		}
	}

	// Held item logic
	if (comp.CurrentItem().IsValidReference())
	{
		Holdable* heldComp = nullptr;
		switch (Holdable::GetHoldable(comp.CurrentItem(), &heldComp))
		{
		case Holdable::HOLDABLE_TYPE::WEAPON:
			reinterpret_cast<ecs::CompHandle<WeaponComponent>>(heldComp)->isPlayerWeapon = true;
			break;
		case Holdable::HOLDABLE_TYPE::NONE:
			// Skip if there is no held item for whatever reason, idk
			return;
		}

		heldComp->isHeld = true;

		comp.CurrentItem()->GetTransform().SetParent(comp.armPivot->GetTransform());
		comp.CurrentItem()->GetTransform().SetLocalPosition((comp.armHoldOffset + heldComp->holdPointPixel));

		comp.CurrentItem()->GetTransform().SetLocalRotation(0.0f);

		Vector2 wLocalScale = comp.CurrentItem()->GetTransform().GetLocalScale();
		if (wLocalScale.x < 0)
		{
			wLocalScale.x = -wLocalScale.x;
			comp.CurrentItem()->GetTransform().SetLocalScale(wLocalScale);
		}

		comp.CurrentItem()->GetComp<EntityLayerComponent>()->SetLayer(ENTITY_LAYER::DEFAULT);

		if (ecs::CompHandle<Physics::PhysicsComp> physComp = comp.CurrentItem()->GetComp<Physics::PhysicsComp>())
		{
			physComp->SetFlag(Physics::PHYSICS_COMP_FLAG::IS_DYNAMIC, false);
		}

		// Quick band-aid to disallow all weapon handling while shielding; Health kit can still be used though
		static bool allowShieldAndGun = true;
		if(allowShieldAndGun)
		{
			std::vector<KEY> allKeys{ heldComp->useKeys };
			for (KEY& k : heldComp->independentUseKeys)
				allKeys.push_back(k);
			for (auto key : allKeys)
			{
				// If -1, they are still holding down the key after the item shuffle
				if (comp.holdState[key] != -1)
				{
					if (Input::GetKeyPressed(key))
					{
						heldComp->OnUseStart(key);
						comp.holdState[key] = 1;
					}
					if (Input::GetKeyCurr(key))
					{
						heldComp->OnUseHold(key);
						comp.holdState[key] = 1;
					}
					else
					{
						heldComp->OnUseEnd(key);
						comp.holdState[key] = 0;
					}
				}
				else if (!Input::GetKeyCurr(key))
				{
					comp.holdState[key] = 0;
				}
			}

		}
		else
		{
			if (!comp.thisEntity->GetCompInChildren<ShieldComponent>()->shieldActivated)
			{
				// Handle using the items
				for (auto key : heldComp->useKeys)
				{
					if (comp.holdState[key] != -1)
					{
						if (Input::GetKeyPressed(key))
						{
							heldComp->OnUseStart(key);
						}
						if (Input::GetKeyCurr(key))
						{
							heldComp->OnUseHold(key);
						}
						else
						{
							heldComp->OnUseEnd(key);
						}
					}
				}
			}
			else
			{
				// Cancel all use as soon as the shield is up
				for (auto key : heldComp->useKeys)
					heldComp->OnUseEnd(key);
			}


			// Handling key uses despite the shield
			for (auto key : heldComp->independentUseKeys)
			{
				if (comp.holdState[key] != -1)
				{
					if (Input::GetKeyPressed(key))
					{
						heldComp->OnUseStart(key);
						comp.holdState[key] = 1;
					}
					if (Input::GetKeyCurr(key))
					{
						heldComp->OnUseHold(key);
						comp.holdState[key] = 1;
					}
					else
					{
						heldComp->OnUseEnd(key);
						comp.holdState[key] = 0;
					}
				}
				else if (!Input::GetKeyCurr(key))
				{
					comp.holdState[key] = 0;
				}
			}
		}

		// Handle reload bar
		ecs::CompHandle<WeaponComponent> weapon = comp.CurrentItem()->GetComp<WeaponComponent>();
		if (weapon)
		{
			comp.reloadBar->SetActive(weapon->isReloading);
			comp.reloadBar->GetCompInChildren<BarComponent>()->SetCurr(weapon->stats.reloadTime - weapon->reloadCurr);
		}
		else
		{
			comp.reloadBar->SetActive(false);
		}

		// Was the item deleted during this update?
		// If it sets isHeld to false we force an inventory pack&update;
		if (!heldComp->isHeld)
		{
			comp.ThrowCurrentItem();
			//comp.CurrentItem() = nullptr;
			comp.PackItemsLeft();
		}
	}
	else
	{
		if (!comp.inventoryUIComponent) return; // Check if reference is valid first
		ecs::CompHandle<InventoryUIManagerComponent> invManager = comp.inventoryUIComponent->GetComp<InventoryUIManagerComponent>();

		// Update currently selected container to be empty
		invManager->SetCurrentlySelectedContainer(comp.currentHoldingItem);
		invManager->SetSelectedContainerDisplaySprite(-1);
	}
}

ecs::EntityHandle PlayerSystem::RaycastItem(const PlayerComponent& comp)
{
	// Calculate player view direction
	const Transform& armPivotTransform{ comp.armPivot->GetTransform() };
	float rot{ armPivotTransform.GetWorldRotation() };
	if (armPivotTransform.GetWorldScale().x < 0.0f)
		rot = 180.0f - rot;
	Vector2 direction{ Vector2::FromAngle(math::ToRadians(rot)) };

	// Raycast
	Physics::RaycastResult raycastResult{};
	Physics::Raycast(armPivotTransform.GetWorldPosition(), direction, { ENTITY_LAYER::WEAPON }, &raycastResult);

	// If we didn't hit anything, or entity is out of range, return nothing.
	if (!raycastResult.hitComp || raycastResult.distance > comp.itemGrabDistance)
		return nullptr;

	return ecs::GetEntity(raycastResult.hitComp);
}

void PlayerSystem::AttemptItemPickup(PlayerComponent& comp, ecs::EntityHandle itemEntity)
{
	ecs::CompHandle<Highlightable> hComp;

	// If hit entity is a lever or intel recorder
	if ((hComp = itemEntity->GetComp<PersistentInteractableComponent>())!=nullptr ||(hComp = itemEntity->GetComp<DefenceStarterComponent>()) != nullptr || (hComp = itemEntity->GetComp<IntelComponent>()) != nullptr)
	{
		if (!hComp->interactable)
			return;
		hComp->OnUseStart(KEY::F); // Simply call the overloaded OnUseStart
		comp.wasItemInteractedWith = true;
	}
	// If hit entity is a weapon or healthpack
	else if ((hComp = itemEntity->GetComp<WeaponComponent>()) != nullptr || (hComp = itemEntity->GetComp<HealthpackComponent>()) != nullptr)
	{
		if (!hComp->interactable)
			return;
		// Add item to the inventory
		int pickedUpSlot{ comp.PickUpItem(static_cast<ecs::CompHandle<Holdable>>(hComp), itemEntity) };
		// Select the item
		comp.ChangeItem(pickedUpSlot);
		// Hide the Pickup UI
		comp.pickupUIComponent->SetActive(false);

		comp.wasItemInteractedWith = true;
	}
}

PlayerComponent::~PlayerComponent()
{
}
