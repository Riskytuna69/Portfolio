/******************************************************************************/
/*!
\file   Character.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   09/25/2024

\author Kendrick Sim Hean Guan (100%)
\par    email: kendrickheanguan.s\@digipen.edu
\par    DigiPen login: kendrickheanguan.s

\brief
  This is the interface file for a component and system that implement a character
  controller.

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
\class CharacterComponent
\brief
	Identifies an entity as player controlled.
*//******************************************************************/
class CharacterComponent : public IRegisteredComponent<CharacterComponent>, ecs::IComponentCallbacks
#ifdef IMGUI_ENABLED
	,IEditorComponent<CharacterComponent>
#endif
{
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	CharacterComponent();

	/*****************************************************************//*!
	\brief
		Copy constructor.
	\param copy
		The CharacterComponent to copy from.
	*//******************************************************************/
	CharacterComponent(const CharacterComponent& copy);

	/*****************************************************************//*!
	\brief
		Move constructor.
	\param other
		The CharacterComponent to steal from.
	*//******************************************************************/
	CharacterComponent(CharacterComponent&& other) noexcept;

	/*****************************************************************//*!
	\brief
		Destructor.
	*//******************************************************************/
	~CharacterComponent();

	/*****************************************************************//*!
	\brief
		Sets whether this entity is grounded or not.
	\param newIsGrounded
		Whether this entity is grounded.
	*//******************************************************************/
	void SetIsGrounded(bool newIsGrounded);


	/*****************************************************************//*!
	\brief
		Gets whether this entity is grounded or not.
	\return
		True if this entity is grounded. False otherwise.
	*//******************************************************************/
	bool GetIsGrounded() const;

	void Jump(const bool&);
	void AimAt(const Vector2&);
	void AimDir(const Vector2&);
	void Dash(const float& power);

	//Getter functions
	const bool& Jumping();
	const float& Speed();
	const float& Accel();
	const float& AirAccel();
	const float& JumpPower();
	const Vector2& AimPos();
	const Vector2& AimDir();

	//Horizontal movement, values exceeding [-1,1] will be clamped
	float horizontalMovement;

public:
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

private:
	/*****************************************************************//*!
	\brief
		Sets grounded-ness depending on collision data.
	\param collisionData
		The data of the collision.
	*//******************************************************************/
	void OnCollision(const Physics::CollisionEventData& collisionData);

private:
	/*****************************************************************//*!
	\brief
		Editor Draw function.
	\param comp
		Component instance.
	*//******************************************************************/
#ifdef IMGUI_ENABLED
	static void EditorDraw(CharacterComponent& comp);
#endif

	//! Whether this entity is grounded or not.
	bool isGrounded;
	bool isJumping;

	//! Variables copied over from Unity
	float speed;
	float accel;
	float airAccel;
	float jumpPower;
	//! Variables for handling aiming due to difference in systems
	Vector2 aimDir;
	Vector2 aimPos;

	property_vtable()
};
property_begin(CharacterComponent)
{
	property_var(speed),
	property_var(accel),
	property_var(airAccel),
	property_var(jumpPower),
}
property_vend_h(CharacterComponent)

/*****************************************************************//*!
\class CharacterSystem
\brief
	Updates the positions of all entities with CharacterComponents based on player input.
*//******************************************************************/
class CharacterSystem : public ecs::System<CharacterSystem, CharacterComponent>
{
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	CharacterSystem();

private:
	/*****************************************************************//*!
	\brief
		Updates a CharacterComponent.
	\param comp
		The CharacterComponent to update.
	*//******************************************************************/
	void UpdateCharacterComp(CharacterComponent& comp);

};
