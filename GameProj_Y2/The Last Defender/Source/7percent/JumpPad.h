/******************************************************************************/
/*!
\file   JumpPad.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   04/02/2025

\author Chan Kuan Fu Ryan (100%)
\par    email: c.kuanfuryan\@digipen.edu
\par    DigiPen login: c.kuanfuryan

\brief
  JumpPad is an ECS component-system pair contains functionality for Jump Pads
  within the game scene.

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#pragma once

// Forward declarations
namespace Physics {
	struct CollisionEventData;
}

/*****************************************************************//*!
\class JumpPadComponent
\brief
	Identifies an entity as a Jump Pad.
*//******************************************************************/
class JumpPadComponent : public IRegisteredComponent<JumpPadComponent>, ecs::IComponentCallbacks
#ifdef IMGUI_ENABLED
	, IEditorComponent<JumpPadComponent>
#endif
{
public:
	bool isActivated;

	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	JumpPadComponent();

	/*****************************************************************//*!
	\brief
		Registers to the collision callback on this entity.
	*//******************************************************************/
	void OnAttached() override;

	/*****************************************************************//*!
	\brief
		Unregisters from the collision callback on this entity.
	*//******************************************************************/
	void OnDetached() override;

	/*****************************************************************//*!
	\brief
		Activates the jump pad when colliding with a PlayerComponent.
	\param collisionData
		The data of the collision.
	*//******************************************************************/
	void OnCollision(const Physics::CollisionEventData& collisionData);

#ifdef IMGUI_ENABLED
	/*****************************************************************//*!
	\brief
		Editor draw function, draws the IMGui elements to allow the
		component's values to be edited. Disabled when IMGui is disabled.
	\param comp
		The component instance.
	*//******************************************************************/
	static void EditorDraw(JumpPadComponent& comp);
#endif
	property_vtable()
};
property_begin(JumpPadComponent)
{
}
property_vend_h(JumpPadComponent)

/*****************************************************************//*!
\class JumpPadSystem
\brief
	Does fixed update on JumpPadComponents.
*//******************************************************************/
class JumpPadSystem : public ecs::System<JumpPadSystem, JumpPadComponent>
{
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	JumpPadSystem();

private:
	/*****************************************************************//*!
	\brief
		Updates JumpPadComponent.
	\param comp
		The JumpPadComponent to update.
	*//******************************************************************/
	void UpdateJumpPadComp(JumpPadComponent& comp);
};