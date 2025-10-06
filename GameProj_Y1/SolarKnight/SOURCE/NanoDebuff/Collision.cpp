/*!************************************************************************
\file       Collision.cpp
\project    Solar Knight
\author(s)  Gavin Ang Jun Liang (ang.g) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/
#include "Collision.h"

namespace Collision {
	bool checkCirclesIntersection(GameObject::GameObject* gameObject1, GameObject::GameObject* gameObject2)
	{
		f32 distance = AEVec2Distance(&gameObject1->position, &gameObject2->position);

		if (distance <= (gameObject1->radius + gameObject2->radius))
			return true;
		else
			return false;
	}

	bool checkSquaresIntersection(GameObject::GameObject* gameObject1, GameObject::GameObject* gameObject2)
	{

		gameObject1->minPosition.x = gameObject1->position.x - (gameObject1->width / 2.0f);
		gameObject1->minPosition.y = gameObject1->position.y - (gameObject1->height / 2.0f);

		gameObject1->maxPosition.x = gameObject1->position.x + (gameObject1->width / 2.0f);
		gameObject1->maxPosition.y = gameObject1->position.y + (gameObject1->height / 2.0f);


		gameObject2->minPosition.x = gameObject2->position.x - (gameObject2->width / 2.0f);
		gameObject2->minPosition.y = gameObject2->position.y - (gameObject2->height / 2.0f);

		gameObject2->maxPosition.x = gameObject2->position.x + (gameObject2->width / 2.0f);
		gameObject2->maxPosition.y = gameObject2->position.y + (gameObject2->height / 2.0f);

		if (gameObject1->maxPosition.x < gameObject2->minPosition.x || 
			gameObject1->minPosition.x > gameObject2->maxPosition.x || 
			gameObject1->maxPosition.y < gameObject2->minPosition.y ||
			gameObject1->minPosition.y > gameObject2->maxPosition.y)
		{
			return false;
		}
		if (gameObject1->maxPosition.x > gameObject2->minPosition.x &&
				(gameObject1->maxPosition.y < gameObject2->minPosition.y &&
				 gameObject1->minPosition.y > gameObject2->maxPosition.y))
		{
			return false;
		}
		if (gameObject1->minPosition.x < gameObject2->minPosition.x &&
				(gameObject1->maxPosition.y < gameObject2->minPosition.y &&
				 gameObject1->minPosition.y > gameObject2->maxPosition.y))
		{
			return false;
		}
	
		return true;

	}

	int checkBinarySquaresIntersection(GameObject::GameObject* gameObject1, GameObject::GameObject* gameObject2)
	{
		int flag = 0;

		gameObject1->minPosition.x = gameObject1->position.x - (gameObject1->width / 2.0f);
		gameObject1->minPosition.y = gameObject1->position.y - (gameObject1->height / 2.0f);

		gameObject1->maxPosition.x = gameObject1->position.x + (gameObject1->width / 2.0f);
		gameObject1->maxPosition.y = gameObject1->position.y + (gameObject1->height / 2.0f);


		gameObject2->minPosition.x = gameObject2->position.x - (gameObject2->width / 2.0f);
		gameObject2->minPosition.y = gameObject2->position.y - (gameObject2->height / 2.0f);

		gameObject2->maxPosition.x = gameObject2->position.x + (gameObject2->width / 2.0f);
		gameObject2->maxPosition.y = gameObject2->position.y + (gameObject2->height / 2.0f);

		if (gameObject1->maxPosition.x < gameObject2->minPosition.x ||
			gameObject1->minPosition.x > gameObject2->maxPosition.x ||
			gameObject1->maxPosition.y < gameObject2->minPosition.y ||
			gameObject1->minPosition.y > gameObject2->maxPosition.y)
		{
			return false;
		}
		if (gameObject1->maxPosition.x > gameObject2->minPosition.x &&
			(gameObject1->maxPosition.y < gameObject2->minPosition.y &&
				gameObject1->minPosition.y > gameObject2->maxPosition.y))
		{
			return false;
		}
		if (gameObject1->minPosition.x < gameObject2->minPosition.x &&
			(gameObject1->maxPosition.y < gameObject2->minPosition.y &&
				gameObject1->minPosition.y > gameObject2->maxPosition.y))
		{
			return false;
		}

		//enemy right collision
		if ((gameObject1->maxPosition.x > gameObject2->minPosition.x &&
			gameObject1->maxPosition.x < gameObject2->position.x - 20.0f) &&
			(gameObject1->maxPosition.y > gameObject2->minPosition.y &&
				gameObject1->minPosition.y < gameObject2->maxPosition.y))
		{
			flag = flag | COLLISION_RIGHT;
			return flag;
		}

		//enemy left collision
		if ((gameObject1->minPosition.x < gameObject2->maxPosition.x &&
			gameObject1->minPosition.x > gameObject2->position.x + 20.0f) &&
			(gameObject1->maxPosition.y > gameObject2->minPosition.y &&
				gameObject1->minPosition.y + 5.0f < gameObject2->maxPosition.y))
		{
			flag = flag | COLLISION_LEFT;
			return flag;
		}

		//enemy btm collision
		if ((gameObject1->minPosition.y < gameObject2->maxPosition.y &&
			gameObject1->minPosition.y > gameObject2->position.y) &&
			(gameObject1->minPosition.x < gameObject2->maxPosition.x &&
				gameObject1->maxPosition.x > gameObject2->minPosition.x))
		{
			flag = flag | COLLISION_BOTTOM;
			return flag;
		}

		//enemy top collision
		if ((gameObject1->maxPosition.y > gameObject2->minPosition.y &&
			gameObject1->maxPosition.y < gameObject2->position.y) &&
			(gameObject1->minPosition.x < gameObject2->maxPosition.x &&
				gameObject1->maxPosition.x > gameObject2->minPosition.x))
		{
			flag = flag | COLLISION_TOP;
			return flag;
		}

		return flag;

	}

	int checkSquaresIntersectionLeftRight(GameObject::GameObject* gameObject1, GameObject::GameObject* gameObject2)
	{

		gameObject1->minPosition.x = gameObject1->position.x - (gameObject1->width / 2.0f);
		gameObject1->minPosition.y = gameObject1->position.y - (gameObject1->height / 2.0f);

		gameObject1->maxPosition.x = gameObject1->position.x + (gameObject1->width / 2.0f);
		gameObject1->maxPosition.y = gameObject1->position.y + (gameObject1->height / 2.0f);


		gameObject2->minPosition.x = gameObject2->position.x - (gameObject2->width / 2.0f);
		gameObject2->minPosition.y = gameObject2->position.y - (gameObject2->height / 2.0f);

		gameObject2->maxPosition.x = gameObject2->position.x + (gameObject2->width / 2.0f);
		gameObject2->maxPosition.y = gameObject2->position.y + (gameObject2->height / 2.0f);

		if (gameObject1->maxPosition.x < gameObject2->minPosition.x ||
			gameObject1->minPosition.x > gameObject2->maxPosition.x ||
			gameObject1->maxPosition.y < gameObject2->minPosition.y ||
			gameObject1->minPosition.y > gameObject2->maxPosition.y)
		{
			return 0;
		}
		if (gameObject1->maxPosition.x > gameObject2->minPosition.x &&
			(gameObject1->maxPosition.y < gameObject2->minPosition.y &&
				gameObject1->minPosition.y > gameObject2->maxPosition.y))
		{
			return 0;
		}
		if (gameObject1->minPosition.x < gameObject2->minPosition.x &&
			(gameObject1->maxPosition.y < gameObject2->minPosition.y &&
				gameObject1->minPosition.y > gameObject2->maxPosition.y))
		{
			return 0;
		}

		//enemy right collision
		if ((gameObject1->maxPosition.x > gameObject2->minPosition.x &&
			gameObject1->maxPosition.x < gameObject2->position.x) &&
			(gameObject1->maxPosition.y > gameObject2->minPosition.y &&
			gameObject1->minPosition.y < gameObject2->maxPosition.y))
		{
			return FLAG_RIGHT;
		}

		//enemy left collision
		if ((gameObject1->minPosition.x < gameObject2->maxPosition.x &&
			gameObject1->minPosition.x > gameObject2->position.x) &&
			(gameObject1->maxPosition.y > gameObject2->minPosition.y &&
			gameObject1->minPosition.y < gameObject2->maxPosition.y))
		{
			return FLAG_LEFT;
		}

		return false;
	}

	int checkSquaresIntersectionUpDown(GameObject::GameObject* gameObject1, GameObject::GameObject* gameObject2)
	{
		gameObject1->minPosition.x = gameObject1->position.x - (gameObject1->width / 2.0f);
		gameObject1->minPosition.y = gameObject1->position.y - (gameObject1->height / 2.0f);

		gameObject1->maxPosition.x = gameObject1->position.x + (gameObject1->width / 2.0f);
		gameObject1->maxPosition.y = gameObject1->position.y + (gameObject1->height / 2.0f);


		gameObject2->minPosition.x = gameObject2->position.x - (gameObject2->width / 2.0f);
		gameObject2->minPosition.y = gameObject2->position.y - (gameObject2->height / 2.0f);

		gameObject2->maxPosition.x = gameObject2->position.x + (gameObject2->width / 2.0f);
		gameObject2->maxPosition.y = gameObject2->position.y + (gameObject2->height / 2.0f);

		if (gameObject1->maxPosition.x < gameObject2->minPosition.x ||
			gameObject1->minPosition.x > gameObject2->maxPosition.x ||
			gameObject1->maxPosition.y < gameObject2->minPosition.y ||
			gameObject1->minPosition.y > gameObject2->maxPosition.y)
		{
			return 0;
		}
		if (gameObject1->maxPosition.x > gameObject2->minPosition.x &&
			(gameObject1->maxPosition.y < gameObject2->minPosition.y &&
				gameObject1->minPosition.y > gameObject2->maxPosition.y))
		{
			return 0;
		}
		if (gameObject1->minPosition.x < gameObject2->minPosition.x &&
			(gameObject1->maxPosition.y < gameObject2->minPosition.y &&
				gameObject1->minPosition.y > gameObject2->maxPosition.y))
		{
			return 0;
		}

		//enemy btm collision
		if ((gameObject1->minPosition.y < gameObject2->maxPosition.y && 
			gameObject1->minPosition.y > gameObject2->position.y) &&
			(gameObject1->minPosition.x < gameObject2->maxPosition.x &&
			gameObject1->maxPosition.x > gameObject2->minPosition.x))
		{
			return FLAG_BOTTOM;
		}

		//enemy top collision
		if ((gameObject1->maxPosition.y > gameObject2->minPosition.y &&
			gameObject1->maxPosition.y < gameObject2->position.y) &&
			(gameObject1->minPosition.x < gameObject2->maxPosition.x &&
			gameObject1->maxPosition.x > gameObject2->minPosition.x))
		{
			return FLAG_TOP;
		}

		return false;
	}

	void snapToPosition(GameObject::GameObject* gameObject1, GameObject::GameObject* gameObject2, int flag)
	{
		switch (flag)
		{
		case Collision_Flag::FLAG_RIGHT:
			gameObject1->position.x -= (gameObject1->maxPosition.x - gameObject2->minPosition.x);
			break;
		case Collision_Flag::FLAG_LEFT:
			gameObject1->position.x += (gameObject2->maxPosition.x - gameObject1->minPosition.x);
			break;
		case Collision_Flag::FLAG_BOTTOM:
			gameObject1->position.y += (gameObject2->maxPosition.y - gameObject1->minPosition.y);
			break;
		case Collision_Flag::FLAG_TOP:
			gameObject1->position.y -= (gameObject1->maxPosition.y - gameObject2->minPosition.y);
			break;
		default:
			break;
		}
	}

	void snapToPositionPlayer(GameObject::GameObject* gameObject1, GameObject::GameObject* gameObject2, Collision_Flag flag)
	{
		switch (flag)
		{
		case Collision_Flag::FLAG_RIGHT:
			(int)(gameObject1->position.x -= (gameObject1->maxPosition.x - gameObject2->minPosition.x));
			break;
		case Collision_Flag::FLAG_LEFT:
			gameObject1->position.x += (gameObject2->maxPosition.x - gameObject1->minPosition.x);
			break;
		case Collision_Flag::FLAG_BOTTOM:
			gameObject1->position.y += ((gameObject2->maxPosition.y - gameObject1->minPosition.y));
			break;
		case Collision_Flag::FLAG_TOP:
			gameObject1->position.y -= (gameObject1->maxPosition.y - gameObject2->minPosition.y);
			break;
		default:
			break;
		}
	}
}