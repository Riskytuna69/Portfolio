/*!************************************************************************
\file       MapLevel.h
\project    Solar Knight
\author(s)  Lau Jia Win (jiawin.lau) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/

#pragma once
#include "MapRoom.h"
#include "MapDoor.h"

#include "GameObject.h"

#include <utility>	// pair
#include <string>	// string
#include <vector>	// vector

namespace Map2 {
	// This should match the csv
	enum class MAP_GRID {
		// Match numbers=================================
		// MUST MATCH NUMBERS WITH CSV
		BG_AIR = 0,
		// COLLISION EXIST
		CL_GRASS = 1,			
		CL_DIRT = 2,
		CL_STONE_PATH = 3,
		CL_STONE_WALL = 4,
		// BG VISUAL
		BG_STONE = 10,	
		BG_CAVE = 11,
		BG_DIRT = 12,


		// Match Letters=================================
		// MUST MATCH LETTERS WITH CSV
		DOOR,				// Door is D(dir)(num) in csv

		
		PLAYER_SPAWN = 20,	// P
		LOOT_SPAWN,			// L

		ENEMY_SPAWN,		// E
		RANGE_SPAWN,		// R
		ELITE_SPAWN,		// EE
		BOSS_SPAWN,			// B

		SAVE_POINT,			// S
		
		MAP_GRID_MAX_NUM
	};
	
	class MapLevel
	{
	public:
		// Constructor
		MapLevel(std::string const& filename);
		// Destructor
		~MapLevel();

		// Prevents other constructors (ONLY use defined constructor)
		MapLevel()								= delete;
		MapLevel(MapLevel const&)				= delete;
		MapLevel& operator=(MapLevel const&)	= delete;

		// Gameplay use ================================================================================
		std::vector<MapRoom*> const& RoomList() const;	// Get
		std::vector<MapDoor*> const& DoorList() const;	// Get

		void		SetActiveRoom(int const& newID);	// Set via ID

		// Room data
		int const&	ActiveRoomID() const;				// Get
		void		GetActiveRoomSize(int& colsX, int& rowsY) const; // Get room size, (Col count, row count)
		int const* const ActiveRoomGrid() const;		// Get
		int const* const ActiveRoomBGGrid() const;		// Get

	private:
		// Create a new room, from given file and id
		MapRoom* LoadRoom(std::string const& filename, int const& newRoomID);
		void LinkDoors();

	
	private:
		std::vector<MapRoom*> roomList;	// List of all rooms in level
		std::vector<MapDoor*> doorList;	// List of all doors in level

		MapRoom* currentlyActiveRoom;	// Currnet Room the game is running in
	};


	// External use ================================================================================

	// Spawn collidable in level
	void SpawnCollidables(MapLevel const* const level, const float& TILE_SIZE, const float& HALF_TILE_SIZE, GameObject::GameObject* const objects);

	// Spawn doors in level
	void SpawnDoors(MapLevel const* const level, const float& TILE_SIZE, const float& HALF_TILE_SIZE, GameObject::GameObject* const objects);

}