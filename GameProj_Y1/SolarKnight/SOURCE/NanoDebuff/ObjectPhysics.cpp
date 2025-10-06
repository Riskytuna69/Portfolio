/*!************************************************************************
\file       ObjectPhysics.cpp
\project    Solar Knight
\author(s)  Min Khant Ko (ko.m) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/

#include "ObjectPhysics.h"

namespace ObjectPhysics {

	void applyGravity(GameObject::GameObject* gameObject, f32 gravityValue) {

		gameObject->position.y -= gravityValue;

	}

	void preventBelowGround(GameObject::GameObject* gameObject) {

		gameObject->minPosition.y = gameObject->position.y - (gameObject->height / 2.0f);

		if (gameObject->minPosition.y < 0) {
			gameObject->minPosition.y = 0;
			gameObject->position.y = gameObject->minPosition.y + (gameObject->height / 2.0f);
		}
	}

	void preventBelowGround(AEVec2 coordinatesXY) {

		if (coordinatesXY.y < 0) {
			coordinatesXY.y = 0;
		}
	}

	void updatePosition(GameObject::GameObject* gameObject, f32 gameDeltaTime) {
		gameObject->position.x += (gameDeltaTime * gameObject->speed) * gameObject->velocity.x;
		gameObject->position.y += (gameDeltaTime * gameObject->speed) * gameObject->velocity.y;
	}

	void applyKnockback(GameObject::GameObject* gameObject, f32 gameDeltaTime) {

		switch (gameObject->knockbackType) 
		{
			case 0: // Knockback to left
			{
				gameObject->position.x -= gameDeltaTime * 500.0f;
				// gameObject->position.x -= (gameDeltaTime * (gameObject->speed * 8)) * abs(gameObject->velocity.x); // Knockback affected by velocity of enemy...
				//gameObject->position.y += (gameDeltaTime * gameObject->speed) * gameObject->velocity.y; // Currently doesn't affect vertical movement
				break;
			}
			case 1: // Knockback to right
			{
				gameObject->position.x += gameDeltaTime * 500.0f;
				//gameObject->position.x += (gameDeltaTime * (gameObject->speed * 8)) * abs(gameObject->velocity.x); // // Knockback affected by velocity of enemy...
				//gameObject->position.y += (gameDeltaTime * gameObject->speed) * gameObject->velocity.y; // Currently doesn't affect vertical movement
				break;
			}
			default:
			{
				// Other knockback types...
				break;
			}
				
		}

		gameObject->knockbackDurationRemaining -= gameDeltaTime;
		if (gameObject->knockbackDurationRemaining <= 0) {
			gameObject->knockbackState = false;
			gameObject->knockbackType = -1; 
		}
	}
}
