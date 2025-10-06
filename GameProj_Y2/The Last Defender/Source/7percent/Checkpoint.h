#pragma once
#include "EntityUID.h"

// Forward declarations
namespace Physics {
	struct CollisionEventData;
}

class CheckpointComponent : public IRegisteredComponent<CheckpointComponent>, ecs::IComponentCallbacks
#ifdef IMGUI_ENABLED
	, IEditorComponent<CheckpointComponent>
#endif
{
public:
	// Serialised values
	float activationTime;
	float lightRadius;
	float revolutionsPerSecond;
	float sineWaveAmplitude;

	// Runtime assigned
	float defaultHologramPosY;
	bool inited;
	EntityReference lightEntity;
	EntityReference hologramEntity;
	float internalTimer;

	CheckpointComponent();

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
		Sets the entity for deletion.
	\param collisionData
		The data of the collision.
	*//******************************************************************/
	void OnCollision(const Physics::CollisionEventData& collisionData);

	void SetState(int state);

	int GetID();

	bool IsActivated();

	void Init();

private:
#ifdef IMGUI_ENABLED
	static void EditorDraw(CheckpointComponent& comp);
#endif
	int ID;
	bool activated;
	AudioReference activatedAudio;

	property_vtable()
};
property_begin(CheckpointComponent)
{
	property_var(ID),
	property_var(activatedAudio),
	property_var(activationTime),
	property_var(lightRadius),
	property_var(revolutionsPerSecond),
	property_var(sineWaveAmplitude)
}
property_vend_h(CheckpointComponent)

class CheckpointSystem : public ecs::System<CheckpointSystem, CheckpointComponent>
{
public:
	CheckpointSystem();
private:
	void UpdateCheckpointComp(CheckpointComponent& comp);
};
