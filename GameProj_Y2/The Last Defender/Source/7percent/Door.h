/******************************************************************************/
/*!
\file   Door.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   03/09/2025

\author Matthew Chan Shao Jie (100%)
\par    email: m.chan\@digipen.edu
\par    DigiPen login: m.chan

\brief
  Interface for Door Component

All content � 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#pragma once
#include "AudioManager.h"

class DoorComponent : public IRegisteredComponent<DoorComponent>, ecs::IComponentCallbacks
#ifdef IMGUI_ENABLED
	, IEditorComponent<DoorComponent>
#endif
{
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	*//******************************************************************/
	DoorComponent();

	/*****************************************************************//*!
	\brief
		Sets the direction of movement.
	\param const Vector2&
		The direction to open/close.
	*//******************************************************************/
	void SetDirection(const Vector2&);
	/*****************************************************************//*!
	\brief
		Gets movement direction.
	\return
		direction.
	*//******************************************************************/
	const Vector2& GetDirection() const;
	/*****************************************************************//*!
	\brief
		Gets movement speed.
	\return
		speed.
	*//******************************************************************/
	float GetSpeed() const;
	/*****************************************************************//*!
	\brief
		Sets movement speed.
	\param _speed
		speed.
	*//******************************************************************/
	void SetSpeed(float _speed);

private:
#ifdef IMGUI_ENABLED
	static void EditorDraw(DoorComponent& comp);
#endif

public:
	/*****************************************************************//*!
	\brief
		Registers to the OnActivated callback on this entity.
	*//******************************************************************/
	void OnAttached() override;

	/*****************************************************************//*!
	\brief
		Unregisters from the OnActivated callback on this entity.
	*//******************************************************************/
	void OnDetached() override;

private:
	/*****************************************************************//*!
	\brief
		Called when the door is activated. Toggles 
	*//******************************************************************/
	void OnActivated();
	void UpdatePositions();
	void Init();


	Vector2 openDirection;
	float speed;
	bool isOpen;
	float openHeight;
	AudioReference openingAudio;
	AudioReference closingAudio;

	Vector2 doorClosedPosition;
	Vector2 doorOpenPosition;
	bool inited;

	property_vtable()
};
property_begin(DoorComponent)
{
	property_var(openDirection),
		property_var(speed),
		property_var(isOpen),
		property_var(openHeight),
		property_var(openingAudio),
		property_var(closingAudio),
}
property_vend_h(DoorComponent)
