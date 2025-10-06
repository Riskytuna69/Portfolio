/******************************************************************************/
/*!
\file   Character.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   09/25/2024

\author Kendrick Sim Hean Guan (100%)
\par    email: kendrickheanguan.s\@digipen.edu
\par    DigiPen login: kendrickheanguan.s

\brief
  This is the source file that implements the component and system as declared in
  Character.h.

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/

#include "Character.h"

#include "RenderComponent.h"
#include "AnimatorComponent.h"
#include "Physics.h"
#include "Collision.h"

CharacterComponent::CharacterComponent()
	: 
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw)
	, 
#endif
	isGrounded{ false }
	, isJumping{ false }
	, horizontalMovement{ 0 }
	, speed{ 0 }
	, accel{0 }
	, airAccel{ 0 }
	, jumpPower{ 0 }
{
}

CharacterComponent::CharacterComponent(const CharacterComponent& copy)
	: 
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw)
	,
#endif
	isGrounded{ copy.isGrounded }
	, isJumping{ copy.isJumping }
	, horizontalMovement{ 0 }
	, speed{ copy.speed }
	, accel{ copy.accel }
	, airAccel{ copy.airAccel }
	, jumpPower{ copy.jumpPower }
{
}

CharacterComponent::CharacterComponent(CharacterComponent&& other) noexcept
	: 
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw)
	, 
#endif
	isGrounded{ other.isGrounded }
	, isJumping{ other.isJumping }
	, horizontalMovement{ 0 }
	, speed{other.speed }
	, accel{other.accel}
	, airAccel{other.airAccel }
	, jumpPower{other.jumpPower }
{
}

CharacterComponent::~CharacterComponent()
{
}

void CharacterComponent::SetIsGrounded(bool newIsGrounded)
{
	isGrounded = newIsGrounded;
}
#ifdef IMGUI_ENABLED
void CharacterComponent::EditorDraw(CharacterComponent& comp)
{
	ImGui::InputFloat("Speed", &comp.speed);
	ImGui::InputFloat("Accel", &comp.accel);
	ImGui::InputFloat("Air Accel", &comp.airAccel);
	ImGui::InputFloat("Jump Power", &comp.jumpPower);
}
#endif
bool CharacterComponent::GetIsGrounded() const
{
	return isGrounded;
}

void CharacterComponent::Jump(const bool &jumping)
{
	isJumping = jumping;
}
void CharacterComponent::AimAt(const Vector2& pos)
{
	aimPos = pos;
}
void CharacterComponent::AimDir(const Vector2& dir)
{
	if (dir.LengthSquared() == 0.0f)
		aimDir = Vector2(1.0f, 0.0f);
	else
		aimDir = dir.Normalize();
}
void CharacterComponent::Dash(const float& power)
{
	ecs::EntityHandle characterEntity{ ecs::GetEntity(this) };
	if (ecs::CompHandle<Physics::PhysicsComp> physComp{ characterEntity->GetComp<Physics::PhysicsComp>() })
	{
		physComp->SetVelocity(aimDir*power);
	}

	//body.velocity = aimDir * Mathf.Sign(flipper.transform.localScale.x) * jumpPower;
}

//Getter function group
const bool& CharacterComponent::Jumping()
{
	return isJumping;
}
const float& CharacterComponent::Speed()
{
	return speed;
}
const float& CharacterComponent::Accel()
{
	return accel;
}
const float& CharacterComponent::AirAccel()
{
	return airAccel;
}
const float& CharacterComponent::JumpPower()
{
	return jumpPower;
}

const Vector2& CharacterComponent::AimPos()
{
	return aimPos;
}

const Vector2& CharacterComponent::AimDir()
{
	return aimDir;
}

void CharacterComponent::OnAttached()
{
	ecs::GetEntity(this)->GetComp<EntityEventsComponent>()->Subscribe("OnCollision", this, &CharacterComponent::OnCollision);
}

void CharacterComponent::OnDetached()
{
	if (auto eventsComp{ ecs::GetEntity(this)->GetComp<EntityEventsComponent>() })
		eventsComp->Unsubscribe("OnCollision", this, &CharacterComponent::OnCollision);
}

void CharacterComponent::OnCollision(const Physics::CollisionEventData& collisionData)
{
	if (!isGrounded)
		SetIsGrounded(collisionData.collisionData->collisionNormal.y > 0.8f);
}

CharacterSystem::CharacterSystem()
	: System_Internal{ &CharacterSystem::UpdateCharacterComp }
{
}

void CharacterSystem::UpdateCharacterComp(CharacterComponent& comp)
{
	ecs::EntityHandle characterEntity{ ecs::GetEntity(&comp) };

	//Movement handling
	float dt{ GameTime::FixedDt() };
	Vector2 movement{};
	comp.horizontalMovement = math::Clamp(comp.horizontalMovement, -1.0f, 1.0f);

	if (ecs::CompHandle<Physics::PhysicsComp> physComp{ characterEntity->GetComp<Physics::PhysicsComp>() })
	{
		if (comp.GetIsGrounded())
		{
			movement.x = comp.horizontalMovement * comp.Accel() * dt;

			//Handle jumping if the Character is trying to jump
			if (comp.Jumping())
			{
				comp.SetIsGrounded(false);
				movement.y = comp.JumpPower();
			}
		}
		else
			movement.x = comp.horizontalMovement * comp.AirAccel() * dt;
		
		//Apply the movement to the physComp
		physComp->AddVelocity(movement);
	}

	//Aim direction handling
	comp.AimDir(comp.AimPos() - characterEntity->GetTransform().GetWorldPosition());

	if (GameTime::NumFixedFrames() > 0)
		comp.SetIsGrounded(false);
}