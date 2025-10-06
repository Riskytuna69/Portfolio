/******************************************************************************/
/*!
\file   DropPodComponent.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   02/13/2025

\author Marc Alviz Evangelista (100%)
\par    email: marcalviz.e\@digipen.edu
\par    DigiPen login: marcalviz.e

\brief

	Component of the drop pod that spawns the actual enemy


All content � 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#include "pch.h"
#include "DropPodComponent.h"
#include "PrefabManager.h"
#include "Scheduler.h"
#include "Messaging.h"
#include "EnemySpawnSystem.h"
#include "EnemyStateMachine.h"

DropPodComponent::DropPodComponent() :
	enemySpawned { false },
	spawnTimer { 0.0f },
	onCollisionSound { "Crash" }
{
}

DropPodComponent::~DropPodComponent()
{
}

void DropPodComponent::OnAttached()
{
	ecs::GetEntity(this)->GetComp<EntityEventsComponent>()->Subscribe("OnCollision", this, &DropPodComponent::OnCollision);

}

void DropPodComponent::OnDetached()
{
	if (auto eventsComp{ ecs::GetEntity(this)->GetComp<EntityEventsComponent>() })
		eventsComp->Unsubscribe("OnCollision", this, &DropPodComponent::OnCollision);
}

void DropPodComponent::OnCollision([[maybe_unused]] const Physics::CollisionEventData& collisionData)
{
	if (!enemySpawned)
	{
		ST<Scheduler>::Get()->Add(0.0f, [thisEntity = ecs::GetEntity(this), onCollisionSound = this->onCollisionSound] {
			ecs::EntityHandle temp = PrefabManager::LoadPrefab("DropPodDisintegratePrefab");
			if (temp)
			{
				temp->GetTransform().SetWorldPosition(thisEntity->GetTransform().GetWorldPosition());
				temp->GetComp<AnimatorComponent>()->Play();
			}
			temp = PrefabManager::LoadPrefab("BossMinion");
			if (temp)
			{
				temp->GetTransform().SetWorldPosition(thisEntity->GetTransform().GetWorldPosition());
				ST<AudioManager>::Get()->StartGroupedSound(onCollisionSound, false, temp->GetTransform().GetWorldPosition(), 4.0f);
				// need randomize the guns the enemy will use
				auto ite = ecs::GetCompsBegin<EnemySpawnerComponent>();
				int weaponLevel = ite->GetGunLevel();
				std::string name = "Weapon_Pistol";

				int weaponIndex = static_cast<int>(util::RandomRange(0, 2));
				switch (weaponLevel)
				{
				case 0:
				{
					switch (weaponIndex)
					{
					case 0:
						name = "Weapon_Pistol";
						break;
					case 1:
						name = "Weapon_SMG";
						break;
					default:
						break;
					}
					break;
				
				}
				case 1:
				{

					switch (weaponIndex)
					{
					case 0:
						name = "Weapon_Shotgun";
						break;
					case 1:
						name = "Weapon_Assault";
						break;
					default:
						break;
					}
					break;
				}
				case 2:
				{

					switch (weaponIndex)
					{
					case 0:
						name = "Weapon_Minigun";
						break;
					case 1:
						name = "Weapon_Sniper";
						break;
					default:
						break;
					}
					break;
				}
				default:
					break;
				}

				temp->GetComp<EnemyControllerComponent>()->weaponPrefabName = name;
				thisEntity->GetComp<DropPodComponent>()->enemySpawned = true;
				thisEntity->SetActive(false);
			}

			});
		
		Messaging::BroadcastAll("MinionSpawned");
	}

}
