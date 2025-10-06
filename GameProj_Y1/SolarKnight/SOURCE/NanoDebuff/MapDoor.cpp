/*!************************************************************************
\file       MapDoor.cpp
\project    Solar Knight
\author(s)  Lau Jia Win (jiawin.lau) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/

#include "MapDoor.h"

namespace Map2 {
	// Constructor
	MapDoor::MapDoor(MapRoom* inLocatedInRoom, int inDoorLinkID, int inPosX, int inPosY, DOOR_EXIT_DIR inExitDir, int inDoorExtraHeight)
		: locatedInRoom{ inLocatedInRoom },
		doorLinkageID{ inDoorLinkID },
		gridPosX{ inPosX },
		gridPosY{ inPosY },
		exitDir{ inExitDir },
		extraHeight{ inDoorExtraHeight },
		linkedToDoor{ nullptr },
		linkedGO{ nullptr } 
	{}

	
	// MapLevel use ================================================================================

	// Returns the linkage ID for this door.
	int const& MapDoor::DoorLinkageID() const
	{
		return doorLinkageID;
	}

	// Sets the linked door to given parameter.
	void MapDoor::LinkedDoor(MapDoor* doorToLink)
	{
		linkedToDoor = doorToLink;
	}

	// Returns the door that this door is linked to.
	MapDoor* const MapDoor::LinkedDoor() const
	{
		return linkedToDoor;
	}


	// Gameplay use ================================================================================

	// Returns the exit direction for this door.
	MapDoor::DOOR_EXIT_DIR const& MapDoor::ExitDirection() const
	{
		return exitDir;
	}

	// Returns the extra height this door has in the game.
	int const& MapDoor::DoorExtraHeight() const
	{
		return extraHeight;
	}

	// Returns the room this door is located in.
	MapRoom* const MapDoor::RoomLocatedIn() const
	{
		return locatedInRoom;
	}

	// Returns the door's position in the given parameters.
	void MapDoor::GetDoorPosXY(int& posX, int& posY) const
	{
		posX = gridPosX;
		posY = gridPosY;
	}


	// Collision check use =========================================================================

	// Assigns this door with the given GameObject
	void MapDoor::AssignedGO(GameObject::GameObject* newGO)
	{
		linkedGO = newGO;
	}

	// Returns the GameObjected assigned to this door.
	GameObject::GameObject const* MapDoor::AssignedGO() const
	{
		return linkedGO;
	}
}