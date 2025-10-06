/*!************************************************************************
\file       MapLevel.cpp
\project    Solar Knight
\author(s)  Lau Jia Win (jiawin.lau) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/

#include "MapLevel.h"

#include "AEVec2.h"
#include "GameRenderer.h"

#include <sstream>	// stringstream
#include <fstream>	// fstream
#include <iostream>	// cout

namespace Map2 {
	namespace {
		const std::string fileBaseDir = "Assets/MapData/";
		const std::string filePostfix = ".csv";

		// Get each line from the csv file and stores in result.
		void GetNextLineAndSplitIntoTokens(std::istream& str, std::vector<std::string>& result)
		{
			// Clear existing data in vector
			result.clear();
			std::string                line;
			std::getline(str, line);

			std::stringstream          lineStream(line);
			std::string                cell;

			while (std::getline(lineStream, cell, ','))
			{
				result.push_back(cell);
			}
		}

		// Returns MapRoom from the given ID.
		MapRoom* GetFromIndex(int const& idMatch, std::vector<MapRoom*> const& _roomList)
		{
			for (MapRoom* r : _roomList)
			{
				if (idMatch == r->RoomID())
					return r;
			}
			return nullptr;
		}

	}

	// Constructor
	MapLevel::MapLevel(std::string const& filename)
		: roomList{},
		doorList{},
		currentlyActiveRoom{ nullptr }
	{
		// Set the full name for "level.csv"
		std::string csvFullName{ fileBaseDir + filename + filePostfix };

		// Open the level csv
		// which contains all rooms csv
		std::ifstream ifs{ csvFullName };
		if (ifs.is_open())
		{
			int roomIDCount{ 0 };				// Keep track room number

			// Stores the room name read
			std::vector<std::string> readRow{};

			// Read all room files
			// which contains room data
			while (ifs.good())
			{
				GetNextLineAndSplitIntoTokens(ifs, readRow);

				// Read the room file
				if (readRow.size() > 0)
				{
					MapRoom* createdRoom = LoadRoom(readRow[0], roomIDCount++);

					if (createdRoom)
					{
						// Successfully read the room
						// Add into level's room list
						
						this->roomList.push_back(createdRoom);
					}
					//else
					//	created room is null!
				}
			}
			ifs.close();

			// Link all the doors in the level
			LinkDoors();

			//return newLevel;
			currentlyActiveRoom = roomList.at(0);
		}
		//else
		//	unable to open file
	}

	// Destructor
	MapLevel::~MapLevel()
	{
		// Delete Room
		for (MapRoom* r : roomList)
		{
			delete r;
			// Room's grid is handled by itself
		}
		
		// Delete Doors
		for (MapDoor* d : doorList)
		{
			delete d;
		}
	}

	// Gameplay use ================================================================================
	
	// Returns List of Rooms in level.
	std::vector<MapRoom*> const& MapLevel::RoomList() const
	{
		return roomList;
	}

	// Returns List of Doors in level.
	std::vector<MapDoor*> const& MapLevel::DoorList() const
	{
		return doorList;
	}

	// Sets current Room to this ID
	void MapLevel::SetActiveRoom(int const& newID)
	{
		MapRoom* tmp = GetFromIndex(newID, roomList);
		if (tmp)
			currentlyActiveRoom = tmp;
		//else
		//	unable to find new room!
	}

	// Returns current Room's ID.
	int const& MapLevel::ActiveRoomID() const
	{
		return currentlyActiveRoom->RoomID();
	}

	// Returns current Room's size.
	void MapLevel::GetActiveRoomSize(int& colsX, int& rowsY) const
	{
		colsX = currentlyActiveRoom->SizeCols();
		rowsY = currentlyActiveRoom->SizeRows();
	}

	// Returns current Room's data grid data
	int const* const MapLevel::ActiveRoomGrid() const
	{
		return currentlyActiveRoom->DataGrid();
	}

	// Returns current Room's BG grid data
	int const* const MapLevel::ActiveRoomBGGrid() const
	{
		return currentlyActiveRoom->BGGrid();
	}


	// Private =====================================================================================

	// Load Room via text file and assigns given ID
	MapRoom* MapLevel::LoadRoom(std::string const& filename, int const& newRoomID)
	{
		// Set the full name for "room.csv"
		std::string csvFullName{ fileBaseDir + filename + filePostfix };

		// Open the file
		std::ifstream ifs{ csvFullName };
		if (ifs.is_open())
		{
			// To store the row's data
			std::vector<std::string> readRow{};

			// Read the rows/cols COUNT (first row)
			GetNextLineAndSplitIntoTokens(ifs, readRow);
			// Reads the first row, which contain max rows/cols data
			int maxRows = std::stoi(readRow[0]);
			int maxCols = std::stoi(readRow[1]);

			// Crete new Room object
			int newRoomSize = maxRows * maxCols;
			if (newRoomSize <= 1) // C6386 warning
			{
				return nullptr;
			}
			// Setup arrays
			int* newRoomGrid = new int[newRoomSize] {0};
			int* newRoomBGGrid = new int[newRoomSize] {0};
			// Create new room
			MapRoom* newRoom = new MapRoom(newRoomGrid, newRoomBGGrid, maxRows, maxCols, newRoomID);
			// counter
			int gridIndex{ 0 };

			// Read the grids
			// Keep track of row/col number
			int rowCount{ 0 }, colCount{ 0 };
			while (ifs.good())
			{
				colCount = { 0 };

				GetNextLineAndSplitIntoTokens(ifs, readRow);
				if (readRow.empty())
					break;

				if (rowCount >= maxRows)
				{
					// csv contains wrong number of rows!
					break;
				}
				// Going through each col (X-axis)
				for (std::string i : readRow)
				{	
					if (colCount >= maxCols)
					{
						// csv contains wrong number of columns!
						break;
					}
					
					// Set grid index
					gridIndex = rowCount * maxCols + colCount;

					// Cell Format:
					// =================
					// Normal Cells (Collision Grid, background colour):
					// [number]
					// 
					// GameObjects (Loot pos, enemy spawn pos):
					// [char][number] -> char determines type, number determines background colour

					switch (i[0])
					{						
					case 'D':
					{
						// Door format
						// Door extra height limit to 0-9 above it, eg height 1 is 2 high door
						// each () represent 1 char
						// D(height-max9)(dir)(id-max-no-limit)
						
						// Is a door, store it as a Door obj
						// Find extra door height (2nd char)
						int doorExtraHeight = std::stoi(i.substr(1, 1));
						// Find door exit direction <>^v (3rd char)
						MapDoor::DOOR_EXIT_DIR exitDir{};
						switch (i[2])
						{
						case '<':
							exitDir = MapDoor::DOOR_EXIT_DIR::EXIT_LEFT;
							break;
						case '>':
							exitDir = MapDoor::DOOR_EXIT_DIR::EXIT_RIGHT;
							break;
						case '^':
							exitDir = MapDoor::DOOR_EXIT_DIR::EXIT_UP;
							break;
						case 'v':
							exitDir = MapDoor::DOOR_EXIT_DIR::EXIT_DOWN;
							break;
						default:
							exitDir = MapDoor::DOOR_EXIT_DIR::EXIT_UNDEFINED;
							break;
						}

						// Find door number (4th char onwards)
						int doorLinkID = std::stoi(i.substr(3));
						// rows - rowcount - 1 ==> flip y axis
						doorList.push_back(new MapDoor(newRoom, doorLinkID, colCount, maxRows - rowCount - 1, exitDir, doorExtraHeight));
						newRoomGrid[gridIndex] = static_cast<int>(MAP_GRID::DOOR);
					}
					break;
					case 'P':
					{
						newRoomGrid[gridIndex] = static_cast<int>(MAP_GRID::PLAYER_SPAWN);
						if (i[1])
						{
							// Find background number
							newRoomBGGrid[gridIndex] = std::stoi(i.substr(1));
						}
						// else no change to bg, bg = 0
					}
					break;
					case 'L':
					{
						newRoomGrid[gridIndex] = static_cast<int>(MAP_GRID::LOOT_SPAWN);

						if (i[1])
						{
							// Find background number
							newRoomBGGrid[gridIndex] = std::stoi(i.substr(1));
						}
						// else no change to bg, bg = 0
					}
					break;
					case 'E':
					{
						if (i[1] == 'E') // EE[number]
						{
							newRoomGrid[gridIndex] = static_cast<int>(MAP_GRID::ELITE_SPAWN);
							if (i[2])
							{
								// Find background number
								newRoomBGGrid[gridIndex] = std::stoi(i.substr(2));
							}
							// else no change to bg, bg = 0
						}
						else // E[number]
						{
							newRoomGrid[gridIndex] = static_cast<int>(MAP_GRID::ENEMY_SPAWN);
							if (i[1])
							{
								// Find background number
								newRoomBGGrid[gridIndex] = std::stoi(i.substr(1));
							}
							// else no change to bg, bg = 0
						}
					}
					break;
					case 'R':
					{
						newRoomGrid[gridIndex] = static_cast<int>(MAP_GRID::RANGE_SPAWN);
						if (i[1])
						{
							// Find background number
							newRoomBGGrid[gridIndex] = std::stoi(i.substr(1));
						}
						// else no change to bg, bg = 0
					}
					break;
					case 'B':
					{
						newRoomGrid[gridIndex] = static_cast<int>(MAP_GRID::BOSS_SPAWN);
						if (i[1])
						{
							// Find background number
							newRoomBGGrid[gridIndex] = std::stoi(i.substr(1));
						}
						// else no change to bg, bg = 0
					}
					break;
					case 'S':
					{
						newRoomGrid[gridIndex] = static_cast<int>(MAP_GRID::SAVE_POINT);
						if (i[1])
						{
							// Find background number
							newRoomBGGrid[gridIndex] = std::stoi(i.substr(1));
						}
						// else no change to bg, bg = 0
					}
					break;
					default:
						// Normal numbers
					{
						// csv not filled with 0!
						if (i == "")
						{
							// Store into grid array
							newRoomGrid[gridIndex] = 0;
						}
						else
						{
							// This stores background to bg map
							int cellIDNum = std::stoi(i);

							switch (cellIDNum)
							{
							case static_cast<int>(MAP_GRID::BG_STONE):
							case static_cast<int>(MAP_GRID::BG_CAVE):
							case static_cast<int>(MAP_GRID::BG_DIRT):
								newRoomBGGrid[gridIndex] = cellIDNum;
								break;

							default:
								// This stores rest number values (collide tile etc) to 
								// Store into grid array
								newRoomGrid[gridIndex] = cellIDNum;
								break;
							}

						}
					}
						break;
					}

					// Incr col offset
					++colCount;
				}
				// Incr row offset
				++rowCount;
			}

			ifs.close();
			return newRoom;
		}

		// unable to open file
		return nullptr;
	}

	// Links all doors together
	void MapLevel::LinkDoors()
	{
		// Go into each room, find all put all doors into list
		// Link doors to each other
		for (MapDoor* d1 : doorList)
		{
			for (MapDoor* d2 : doorList)
			{
				if (d1 == d2)	// Ignore if self
					continue;

				if (d1->DoorLinkageID() == d2->DoorLinkageID())
				{
					// Link doors
					d1->LinkedDoor(d2);
					d2->LinkedDoor(d1);
				}
			}
		}
	}


	// External use ================================================================================

	void SpawnCollidables(MapLevel const* const level, const float& TILE_SIZE, const float& HALF_TILE_SIZE, GameObject::GameObject* const objects)
	{
		// Read room again and create grids
		static int nextRoomMaxRows{}, nextRoomMaxCols{};
		level->GetActiveRoomSize(nextRoomMaxCols, nextRoomMaxRows);
		const int* roomGrid = level->ActiveRoomGrid();

		GameObject::GameObject* tmpMapSpawn{ nullptr };
		AEVec2 tmpMapPos{ 0,0 };
		int textListNum{ 0 };

		// create grids
		for (int row{ 0 }; row < nextRoomMaxRows; ++row)
		{
			for (int col{ 0 }; col < nextRoomMaxCols; ++col)
			{
				MAP_GRID gridVal = static_cast<MAP_GRID>(roomGrid[row * nextRoomMaxCols + col]);

				// Get textures
				switch (gridVal)
				{
				case MAP_GRID::CL_GRASS:
					textListNum = GR::GameRenderer::T_MAP_GRASS; break;
				case MAP_GRID::CL_DIRT:
					textListNum = GR::GameRenderer::T_MAP_DIRT; break;
				case MAP_GRID::CL_STONE_PATH:
					textListNum = GR::GameRenderer::T_MAP_STONE_PATH; break;
				case MAP_GRID::CL_STONE_WALL:
					textListNum = GR::GameRenderer::T_MAP_STONE_WALL; break;
				default:
					break;
				}

				// Set if collidable
				switch (gridVal)
				{
				case MAP_GRID::CL_GRASS:
				case MAP_GRID::CL_DIRT:
				case MAP_GRID::CL_STONE_PATH:
				case MAP_GRID::CL_STONE_WALL:

					if ((tmpMapSpawn = GameObject::GetGameObject(GameObject::OBJ_GRID, objects)) != nullptr)
					{
						// rows - rowcount - 1 ==> flip y axis
						AEVec2Set(&tmpMapPos,
							col * TILE_SIZE + HALF_TILE_SIZE,
							(nextRoomMaxRows - row - 1) * TILE_SIZE + HALF_TILE_SIZE);
							*tmpMapSpawn = GameObject::CreateGameObject(
							/*type of obj*/		GameObject::OBJ_GRID,
							/*position*/		tmpMapPos,
							/*width of obj*/	TILE_SIZE,
							/*height of obj*/	TILE_SIZE,
							/*active*/			true,
							/*Max HP*/			0,
							/*Speed*/			0.0f,
							/*Radius*/			0.0f,
							/*Lifetime*/		0.0f,
							/*levelNumber*/		textListNum // borrowing level num
						);
					}
					break;
				default:
					break;
				}
			}
		}
	}

	void SpawnDoors(MapLevel const* const level, const float& TILE_SIZE, const float& HALF_TILE_SIZE, GameObject::GameObject* const objects)
	{
		GameObject::GameObject* tmpMapSpawn{ nullptr };
		AEVec2 tmpMapPos{ 0,0 };
		int currRoomID = level->ActiveRoomID();

		// generate door objs
		for (Map2::MapDoor* d : level->DoorList())
		{
			int x{}, y{};
			d->GetDoorPosXY(x, y);
			int doorRoomID{ d->RoomLocatedIn()->RoomID() };
			int extraHeight{ d->DoorExtraHeight() };
			if ((tmpMapSpawn = GameObject::GetGameObject(GameObject::OBJ_DOOR, objects)) != nullptr)
			{

				AEVec2Set(&tmpMapPos,
					x * TILE_SIZE + HALF_TILE_SIZE,
					y * TILE_SIZE + HALF_TILE_SIZE + extraHeight * HALF_TILE_SIZE);
				*tmpMapSpawn = GameObject::CreateGameObject(
					/*type of obj*/		GameObject::OBJ_DOOR,
					/*position*/		tmpMapPos,
					/*width of obj*/	TILE_SIZE,
					/*height of obj*/	TILE_SIZE + TILE_SIZE * extraHeight,
					/*active*/			(currRoomID == doorRoomID ? true : false),
					/*Max HP*/			0,
					/*Speed*/			0.0f,
					/*Radius*/			0.0f,
					/*Lifetime*/		0.0f,
					/*levelNumber*/		doorRoomID
				);
				d->AssignedGO(tmpMapSpawn);
			}
		}
	}

}