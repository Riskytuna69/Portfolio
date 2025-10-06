/******************************************************************************/
/*!
\file   EnemyStateMachine.ipp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 4
\date   02/03/2025

\author Kendrick Sim Hean Guan (100%)
\par    email: kendrickheanguan.s\@digipen.edu
\par    DigiPen login: kendrickheanguan.s


\brief
	Template implementation file for Enemy AI.

All content © 2025 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/

#pragma once
#include "EnemyStateMachine.h"
#include "AnimatorComponent.h"

namespace sm {

	template<typename T>
	void ActivityEnemyAttackBase<T>::PerformChase(ecs::CompHandle<EnemyControllerComponent> enemyComp, const Vector2& targetLocation, bool maintainMinDist)
	{
		Transform& enemyTransform{ ecs::GetEntityTransform(enemyComp) };
		float distXToTarget{ targetLocation.x - enemyTransform.GetWorldPosition().x };
		float posDistXToTarget{ std::fabsf(distXToTarget) };
		if (posDistXToTarget == 0.0f)
		{
			// Reset animation frame just in case
			if (ecs::CompHandle<AnimatorComponent> animComp{ enemyComp->entitySprite->GetComp<AnimatorComponent>() })
				animComp->Reset();
			return;
		}

		float movementX{};
		if (maintainMinDist)
		{
			if (posDistXToTarget > enemyComp->uniqueMaxMaintainDistance)
				movementX = distXToTarget / posDistXToTarget;
			else if (posDistXToTarget < enemyComp->uniqueMinMaintainDistance)
				movementX = -distXToTarget / posDistXToTarget;
		}
		else
			movementX = distXToTarget / posDistXToTarget;

		if (movementX != 0.0f)
			enemyTransform.AddWorldPosition({ movementX * enemyComp->GetSpeed() * GameTime::FixedDt(), 0.0f });

		// Play/Pause the enemy animation depending on movement.
		if (ecs::CompHandle<AnimatorComponent> animComp{ enemyComp->entitySprite->GetComp<AnimatorComponent>() })
			if (movementX != 0.0f)
				animComp->Play();
			else
			{
				animComp->Pause();
				animComp->Reset();
			}
	}

	template<typename T> 
	void ActivityEnemyAttackBase<T>::PerformSuicideChase(ecs::CompHandle<EnemyControllerComponent> enemyComp, const Vector2& targetLocation, bool maintainMinDist)
	{
		//To be modified in the future, exact same as PerformChase() for now
		Transform& enemyTransform{ ecs::GetEntityTransform(enemyComp) };
		float distXToTarget{ targetLocation.x - enemyTransform.GetWorldPosition().x };
		float posDistXToTarget{ std::fabsf(distXToTarget) };
		if (posDistXToTarget == 0.0f)
			return;

		float movementX{};
		if (maintainMinDist)
		{
			if (posDistXToTarget > enemyComp->uniqueMaxMaintainDistance/2.f)
				movementX = distXToTarget / posDistXToTarget;
			else if (posDistXToTarget < enemyComp->uniqueMinMaintainDistance/2.f)
				movementX = -distXToTarget / posDistXToTarget;
		}
		else
			movementX = distXToTarget / posDistXToTarget;



		if (movementX != 0.0f)
			enemyTransform.AddWorldPosition({ movementX * enemyComp->GetSpeed() * 2.f * GameTime::FixedDt(), 0.0f });

		// Play/Pause the enemy animation depending on movement.
		if (ecs::CompHandle<AnimatorComponent> animComp{ enemyComp->entitySprite->GetComp<AnimatorComponent>() })
			if (movementX != 0.0f)
				animComp->Play();
			else
			{
				animComp->Pause();
				animComp->Reset();
			}
	}

	template<typename T>
	void ActivityEnemyAttackBase<T>::UpdateArmPivot(ecs::CompHandle<EnemyControllerComponent> enemyComp, const Vector2& targetLocation)
	{
		if (ecs::CompHandle<ArmPivotComponent> armPivot{ ecs::GetEntity(enemyComp)->GetCompInChildren<ArmPivotComponent>() })
			armPivot->RotateTowards(targetLocation);

		// Bug fix: The player might be out of LOS but not far enough for the arm pivot to change direction.
		// We'll need to flip the enemy's direction ourselves to ensure the player stays within LOS...
		if (enemyComp->GetViewDirection().x * (targetLocation.x - ecs::GetEntityTransform(enemyComp).GetWorldPosition().x) < 0.0f)
			enemyComp->FlipViewDirection();
	}

	template<typename NextStateType>
	TransitionIfEnemyIsType::TransitionIfEnemyIsType(ENEMY_ROLE role, const NextStateTypeStruct<NextStateType>& dummy)
		: ECSTransitionBase{ dummy }
		, role{ role }
	{
	}

	template <typename NextStateType>
	TransitionEnemyPlayerInLOS::TransitionEnemyPlayerInLOS(bool checkForInLOS, const NextStateTypeStruct<NextStateType>& dummy)
		: ECSTransitionBase{ dummy }
		, checkForInLOS{ checkForInLOS }
	{
	}

	template<typename NextStateType>
	TransitionEnemyPlayerTooClose::TransitionEnemyPlayerTooClose(const NextStateTypeStruct<NextStateType>& dummy)
		: ECSTransitionBase{ dummy }
	{
	}

	// Transition - Enemy is fully alerted (detection level == 1.0f)
	template <typename NextStateType>
	TransitionEnemyFullyAlerted::TransitionEnemyFullyAlerted(const NextStateTypeStruct<NextStateType>& dummy)
		: ECSTransitionBase{ dummy }
	{
	}

	//Transition - Enemy is fully calm (detection level == 0.0f)
	template <typename NextStateType>
	TransitionEnemyNoSuspicion::TransitionEnemyNoSuspicion(const NextStateTypeStruct<NextStateType>& dummy)
		: ECSTransitionBase{ dummy }
	{
	}

	template <typename NextStateType>
	TransitionEnemyStunned::TransitionEnemyStunned(bool checkIsStunned, const NextStateTypeStruct<NextStateType>& dummy)
		: ECSTransitionBase{ dummy }
		, checkIsStunned{ checkIsStunned }
	{
	}

}
