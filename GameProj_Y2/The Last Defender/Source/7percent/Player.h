/******************************************************************************/
/*!
\file   Player.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   01/31/2025

\author Matthew Chan Shao Jie (100%)
\par    email: m.chan\@digipen.edu
\par    DigiPen login: m.chan

\brief
  This interface has been hijacked by Matthew to handle the Player's inventory

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#pragma once
#include "Highlightable.h"
#include "EntityUID.h"
#include "AudioManager.h"

// Forward declarations
class Holdable;
namespace Physics {
	struct CollisionEventData;
}

#define MAX_ITEM_COUNT 5

/*****************************************************************//*!
\class PlayerComponent
\brief
	Identifies an entity as player controlled.
*//******************************************************************/
class PlayerComponent : public IRegisteredComponent<PlayerComponent>, public ecs::IComponentCallbacks
#ifdef IMGUI_ENABLED
	, IEditorComponent<PlayerComponent>
#endif
{
public:
	PlayerComponent();
	~PlayerComponent();

	void Init();

	/*****************************************************************//*!
	\brief
		Adds an item to the first free inventory slot. If none are available,
		replaces the currently selected slot with the item, throwing the
		previous item away.
	\param item
		The item to pick up.
	\param itemEntity
		The entity holding this item.
	\return
		The index that the item was added to.
	*//******************************************************************/
	int PickUpItem(ecs::CompHandle<Holdable> item, ecs::EntityHandle itemEntity);

	/*****************************************************************//*!
	\brief
		Throws the currently selected item if it exists. Does not update
		the currently selected slot.
	*//******************************************************************/
	void ThrowCurrentItem();

	/*****************************************************************//*!
	\brief
		Moves items to fill gaps towards the left. May change the currently
		selected item.
	*//******************************************************************/
	void PackItemsLeft();

	/*****************************************************************//*!
	\brief
		Blocks all use keys until the player releases them.
	*//******************************************************************/
	void BlockUseKeys();

	/*****************************************************************//*!
	\brief
		Register OnCollision.
	*//******************************************************************/
	void OnAttached() override;

	/*****************************************************************//*!
	\brief
		Unregister OnCollision.
	*//******************************************************************/
	void OnDetached() override;

	/*****************************************************************//*!
	\brief
		Checks if there are empty item slots.
	*//******************************************************************/
	bool HasEmptySlots();

	/*****************************************************************//*!
	\brief
		On collision.
	*//******************************************************************/
	void OnCollision(const Physics::CollisionEventData& collisionData);

	/*****************************************************************//*!
	\brief
		Gets a Holdable component from the currently held item.
	*//******************************************************************/
	ecs::CompHandle<Holdable> CurrentHoldable();

	EntityReference& CurrentItem();
	void ChangeItem(int);
	void CurrentHoldableSwitch(bool);
	void ChangeItem(bool);
	void UpdateHeldItems();

	bool wasItemInteractedWith;
	bool wasItemThrown;
	bool wasItemSwitched;
	int itemGrabDistance;
	int unlockedSlots;
	float itemSwitchCooldown;
	EntityReference reloadBar;
	EntityReference heldItems[5];
	EntityReference lastHighlightedEntity;
	EntityReference armPivot;
	EntityReference weaponGrabber;

	// This may save some processing in future
	EntityReference thisEntity;

	const Vector2 armHoldOffset{ 17.0f, -3.0f };

	// These are not to be serialized
	bool inited;
	int currentHoldingItem = 0;
	float currentSwitchCooldown;

	EntityReference inventoryUIComponent;
	EntityReference pickupUIComponent;

	EntityReference itemInfoPanelEntity;

	AudioReference audioThrow;
	AudioReference audioHolster;
	AudioReference audioInvalid;
	// 0 = not held, 1 = held, -1 = held after item change
	std::unordered_map<KEY, int> holdState;

private:

#ifdef IMGUI_ENABLED
	static void EditorDraw(PlayerComponent& comp);
#endif

	property_vtable()
};
property_begin(PlayerComponent)
{
	property_var(itemGrabDistance),
	property_var(reloadBar),
	property_var(unlockedSlots),
	property_var(itemSwitchCooldown),
	property_var(inventoryUIComponent),
	property_var(pickupUIComponent),
	property_var(itemInfoPanelEntity),
	property_var(weaponGrabber),
	property_var(audioThrow),
	property_var(audioHolster),
	property_var(audioInvalid)
}
property_vend_h(PlayerComponent)
#pragma endregion

class PlayerSystem : public ecs::System<PlayerSystem, PlayerComponent>
{
public: 
	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	PlayerSystem();

private:
	/*****************************************************************//*!
	\brief
		Updates a PlayerSystem.
	\param comp
		The player component to update.
	*//******************************************************************/
	void UpdatePlayerComp(PlayerComponent& comp);

	/*****************************************************************//*!
	\brief
		Casts a ray in the direction of the player's view and returns any
		hit entity on the WEAPON layer.
	\param comp
		The player component.
	\return
		The hit entity.
	*//******************************************************************/
	ecs::EntityHandle RaycastItem(const PlayerComponent& comp);

	/*****************************************************************//*!
	\brief
		Attempts to pick up an item from the world.
	\param comp
		The player component.
	\param itemEntity
		The item entity.
	*//******************************************************************/
	void AttemptItemPickup(PlayerComponent& comp, ecs::EntityHandle itemEntity);
};
