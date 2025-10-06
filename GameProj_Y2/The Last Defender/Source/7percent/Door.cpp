/******************************************************************************/
/*!
\file   Door.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   03/09/2025

\author Matthew Chan Shao Jie (100%)
\par    email: m.chan\@digipen.edu
\par    DigiPen login: m.chan

\brief
  Implementation of scene transitions

All content � 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#include "pch.h"
#include "pch.h"
#include "Door.h"
#include "TweenManager.h"

DoorComponent::DoorComponent():
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
	openDirection{0.0f,1.0f},
speed{10.0f},
isOpen{false},
openHeight{150.0f},
openingAudio{ "" },// TODO: Door sounds?
closingAudio{ "" },
inited{false}
{
}

void DoorComponent::SetDirection(const Vector2& _openDirection)
{
	openDirection = _openDirection;
}

const Vector2& DoorComponent::GetDirection() const
{
	return openDirection;
}

float DoorComponent::GetSpeed() const
{
	return speed;
}

void DoorComponent::SetSpeed(float _speed)
{
	speed = _speed;
}

#ifdef IMGUI_ENABLED
void DoorComponent::EditorDraw(DoorComponent& comp)
{
	Vector2 openDirection;
	// Helper function for vector2 controls (shamelessly copied over from Editor.cpp)
	auto DrawVec2Control = [](const char* label, Vector2& values, float columnWidth, float speed, const char* format)
		{
			bool modified = false;
			ImGui::PushID(label);
			ImGui::Columns(2, nullptr, false);
			ImGui::SetColumnWidth(0, columnWidth);
			ImGui::Text(label);
			ImGui::NextColumn();

			// X Component
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
			ImGui::Text("X");
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::PushItemWidth(70);
			modified |= ImGui::DragFloat("##X", &values.x, speed, 0.0f, 0.0f, format);
			ImGui::PopItemWidth();
			ImGui::SameLine();

			// Y Component
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 1.0f, 0.4f, 1.0f));
			ImGui::Text("Y");
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::PushItemWidth(70);
			modified |= ImGui::DragFloat("##Y", &values.y, speed, 0.0f, 0.0f, format);
			ImGui::PopItemWidth();

			ImGui::Columns(1);
			ImGui::PopID();
			return modified;
		};
	DrawVec2Control("Open Direction", comp.openDirection, 80.0f, 1.0f, "%.1f");

	ImGui::DragFloat("Opening Speed", &comp.speed);
	ImGui::Checkbox("Is Opened", &comp.isOpen);
	ImGui::DragFloat("Open Height", &comp.openHeight);

	comp.openingAudio.EditorDraw("Open Sound");
	comp.closingAudio.EditorDraw("Close Sound");
	if(ImGui::Button("Update Door Positions"))
	comp.UpdatePositions();
}
#endif

void DoorComponent::OnAttached()
{
	ecs::EntityHandle thisEntity = ecs::GetEntity(this);

	thisEntity->GetComp<EntityEventsComponent>()->Subscribe("OnActivated", this, &DoorComponent::OnActivated);

	// Manually set the closed/open positions
	UpdatePositions();
}

void DoorComponent::OnDetached()
{
	if (auto eventsComp{ ecs::GetEntity(this)->GetComp<EntityEventsComponent>() })
		eventsComp->Unsubscribe("OnActivated", this, &DoorComponent::OnActivated);

}

void DoorComponent::OnActivated()
{
	if (!inited)
		Init();

	ecs::EntityHandle thisEntity = ecs::GetEntity(this);
	// Remove the Tween Component if this entity contains one
	// Note from Matthew: Not sure if the sanity check is required, will leave this here for now
	if (auto tweenComp{ thisEntity->GetComp<TweenComponent>() })
	{
		//thisEntity->RemoveComp<TweenComponent>();
	}

	// Get progress here
	isOpen = !isOpen;

	// Sanity check for speed here
	if (speed <= 0.0f)
		return;
	float progress = (thisEntity->GetTransform().GetWorldPosition() - doorClosedPosition).Length() / openHeight;
	float moveDuration = (isOpen ? 1.0f - progress : progress) * openHeight / speed;
	ST<TweenManager>::Get()->StartTween(thisEntity,
		&Transform::SetWorldPosition,
		thisEntity->GetTransform().GetWorldPosition(),
		isOpen ? doorOpenPosition : doorClosedPosition,											// Self-explanatory, position is dictated by whether we're opening or closing
		moveDuration,	// We choose the speed multiplied by the progress or inverse, again based on opening or closing
		TT::EASE_OUT);																			// Ease out for that SMOOOOOOOOTH finish
}

void DoorComponent::UpdatePositions()
{
	ecs::EntityHandle thisEntity = ecs::GetEntity(this);
	if (isOpen)
	{
		doorOpenPosition = thisEntity->GetTransform().GetWorldPosition();
		doorClosedPosition = doorOpenPosition - openDirection.Normalize() * openHeight;
	}
	else {
		doorClosedPosition = thisEntity->GetTransform().GetWorldPosition();
		doorOpenPosition = doorClosedPosition + openDirection.Normalize() * openHeight;
	}

}

void DoorComponent::Init()
{
	inited = true;
	UpdatePositions();
}
