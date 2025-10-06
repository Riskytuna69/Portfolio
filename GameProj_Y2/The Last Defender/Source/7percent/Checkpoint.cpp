#include "Checkpoint.h"
#include "Physics.h"
#include "Collision.h"
#include "Player.h"
#include "GameManager.h"
#include "AnimatorComponent.h"
#include "Scheduler.h"
#include "LightComponent.h"
#include "TweenManager.h"
#include "NameComponent.h"

CheckpointComponent::CheckpointComponent() :
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
	activationTime{ 0.2f },
	lightRadius{ 150.0f },
	revolutionsPerSecond{ 1.0f },
	sineWaveAmplitude{ 10.0f },
	defaultHologramPosY{ 15.0f },
	inited{ false },
	lightEntity{ nullptr },
	hologramEntity{ nullptr },
	internalTimer{ 0.0f },
	ID{ 0 },
	activated{ false },
	activatedAudio{ "Checkpoint_Activate" }
{
}

void CheckpointComponent::OnAttached()
{
	if (ecs::GetCurrentPoolId() != ecs::POOL::DEFAULT)
		return;

	// Lord Kendrick pls do not smite me
	EntityReference thisEntity = ecs::GetEntity(this);
	ST<Scheduler>::Get()->Add([thisEntity]() mutable {
		if (thisEntity == nullptr)
			return;
		ecs::EntityHandle colliderEntity = ecs::GetEntity(thisEntity->GetCompInChildren<Physics::ColliderComp>());
		colliderEntity->GetComp<EntityEventsComponent>()->Subscribe("OnCollision", thisEntity->GetComp<CheckpointComponent>(), &CheckpointComponent::OnCollision);
		});
}

void CheckpointComponent::OnDetached()
{
	auto thisEntity = ecs::GetEntity(this);
	if (auto colliderComp{ thisEntity->GetCompInChildren<Physics::ColliderComp>() })
	{
		ecs::EntityHandle colliderEntity = ecs::GetEntity(colliderComp);
		if (auto eventsComp{ colliderEntity->GetComp<EntityEventsComponent>() })
			eventsComp->Unsubscribe("OnCollision", this, &CheckpointComponent::OnCollision);
	}
}

void CheckpointComponent::OnCollision(const Physics::CollisionEventData& collisionData)
{
	// Don't bother with any logic if this was activated already
	if (activated)
		return;

	// Get the other entity
	ecs::EntityHandle otherEntity = ecs::GetEntity(collisionData.otherComp);

	// Get the Player Component
	ecs::CompHandle<PlayerComponent> playerComp = otherEntity->GetComp<PlayerComponent>();
	if (playerComp != nullptr)
	{
		// Get this
		ecs::EntityHandle thisEntity = ecs::GetEntity(this);

		// Update the Game Manager
		if(ST<GameManager>::Get()->OnCheckpointActivated(ID, thisEntity))
		
		// Play Audio if not the first checkpoint
		if (ID != 0)
		{
			ST<AudioManager>::Get()->StartSound(activatedAudio);
		}
	}
}

void CheckpointComponent::SetState(int state)
{
	// Get this
	ecs::EntityHandle thisEntity = ecs::GetEntity(this);

	// Get the multisprite comp
	ecs::CompHandle<LightComponent> lightComp = lightEntity->GetComp<LightComponent>();

	// Safety checks
	if (!lightComp) return;

	lightComp->SetRadius(0.0f);
	switch (state)
	{
	case 0:
		// Make it usable again, just in case
		thisEntity->GetComp<EntityLayerComponent>()->SetLayer(ENTITY_LAYER::PLAYERDETECTOR);
		
		// Set as activated
		activated = false;

		// Lower light radius and deactivate entities
		lightComp->SetRadius(0.0f);
		lightEntity->SetActive(false);
		hologramEntity->SetActive(false);
		break;
	case 1:
	case 2:
		// Make it untouchable
		thisEntity->GetComp<EntityLayerComponent>()->SetLayer(ENTITY_LAYER::DEFAULT);
		
		// Set as activated
		activated = true;

		// Set entities to active
		lightEntity->SetActive(true);
		hologramEntity->SetActive(true);

		// Tween light radius
		ST<TweenManager>::Get()->StartTween(
			ecs::GetEntity(lightComp),
			&LightComponent::SetRadius,
			0.0f,
			lightRadius,
			activationTime,
			TT::EASE_OUT);
		break;
	}
}

int CheckpointComponent::GetID()
{
	return ID;
}

bool CheckpointComponent::IsActivated()
{
	return activated;
}

void CheckpointComponent::Init()
{
	auto children = ecs::GetEntity(this)->GetTransform().GetChildren();

	// Find light and hologram references
	for (auto child : children)
	{
		ecs::EntityHandle childentity = child->GetEntity();
		std::string childName = childentity->GetComp<NameComponent>()->GetName();

		if (childName == "Light")
		{
			lightEntity = childentity;
		}
		if (childName == "Hologram")
		{
			hologramEntity = childentity;
		}
	}

	// Check existence
	if (!lightEntity || !hologramEntity)
	{
		CONSOLE_LOG(LEVEL_WARNING) << "Checkpoint does not have all necessary references!";
		return;
	}

	// Get the default scale X of hologram
	auto hologramTransform = hologramEntity->GetTransform();
	defaultHologramPosY = hologramTransform.GetLocalPosition().y;
	internalTimer = 1.0f;

	inited = true;
}

#ifdef IMGUI_ENABLED
void CheckpointComponent::EditorDraw(CheckpointComponent& comp)
{
	ImGui::InputInt("ID", &comp.ID);
	comp.activatedAudio.EditorDraw("Activated Audio");
	ImGui::DragFloat("Activation Time", &comp.activationTime);
	ImGui::DragFloat("Light Radius", &comp.lightRadius);
	ImGui::DragFloat("Revolutions Per Second", &comp.revolutionsPerSecond);
	ImGui::DragFloat("Sine Wave Amplitude", &comp.sineWaveAmplitude);
}
#endif

CheckpointSystem::CheckpointSystem()
	: System_Internal{ &CheckpointSystem::UpdateCheckpointComp }
{
}

void CheckpointSystem::UpdateCheckpointComp(CheckpointComponent& comp)
{
	// Init
	if (!comp.inited)
	{
		comp.Init();
	}

	// Only perform tweening anims if activated
	if (comp.IsActivated())
	{
		comp.internalTimer += GameTime::FixedDt() * comp.revolutionsPerSecond;

		float offsetY = comp.sineWaveAmplitude * sinf(comp.internalTimer * 2.0f * math::PI_f);
		comp.hologramEntity->GetTransform().SetLocalPosition({ 0.0f, comp.defaultHologramPosY + offsetY});

		if (comp.internalTimer > 1.0f)
		{
			Vector2 currentScale = comp.hologramEntity->GetTransform().GetLocalScale();

			ST<TweenManager>::Get()->StartTween(
				comp.hologramEntity,
				&Transform::SetLocalScale,
				currentScale,
				Vector2{ -currentScale.x, currentScale.y },
				1.0f / comp.revolutionsPerSecond,
				TT::EASE_BOTH);
			comp.internalTimer = 0.0f;
		}
	}
}
