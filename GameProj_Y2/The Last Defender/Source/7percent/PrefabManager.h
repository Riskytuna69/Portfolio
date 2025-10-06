/******************************************************************************/
/*!
\file   PrefabManager.h
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   11/29/2024

\author Matthew Chan Shao Jie (50%)
\par    email: m.chan\@digipen.edu
\par    DigiPen login: m.chan

\brief
  This file contains the declaration for the Prefab Manager, which handles
  loading and saving of prefabs.

  All functions are also defined inline within this class.

All content � 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#pragma once
#include "GameSettings.h"

class PrefabManager
{
	friend ST<PrefabManager>;//! Declare this class as a singleton
	std::vector<std::string> _allPrefabs;
	std::map<std::string, ecs::EntityHandle>_prefabPool;
private:
	static const std::string& FolderDir()
	{
		return ST<Filepaths>::Get()->prefabsSave;
	}
public:
	PrefabManager() {
		Update();
	}
	static bool DeletePrefab(std::string name)
	{
		// Ensure directory is created
		if (!std::filesystem::exists(FolderDir()) && !std::filesystem::create_directory(FolderDir()))
		{
			CONSOLE_LOG(LEVEL_ERROR) << "Failed to create prefabs directory!";
			return false;
		}

		// Delete the file if it exists
		if (std::filesystem::exists(FolderDir() + "/" + name + ".prefab"))
		{
			std::remove((FolderDir() + "/" + name + ".prefab").c_str());
			// Update just this prefab afterwards
			ST<PrefabManager>::Get()->Update(name);

			return true;
		}

		return false;

	}
	static bool SavePrefab(ecs::EntityHandle entity, std::string name)
	{
		// Ensure directory is created (ty Kendrick)
		if (!std::filesystem::exists(FolderDir()) && !std::filesystem::create_directory(FolderDir()))
		{
			CONSOLE_LOG(LEVEL_ERROR) << "Failed to create prefabs directory!";
			return false;
		}
		Serializer serializer{ FolderDir() + "/" + name + ".prefab" };

		// Save Children
		SaveChildOfPrefab(entity, serializer);

		serializer.SaveAndClose();

		// Update the Prefabs list 
		ST<PrefabManager>::Get()->Update(name);
		return true;
	}
	static bool SaveChildOfPrefab(ecs::EntityHandle entity, Serializer& serializer)
	{
		serializer.Serialize(entity);

		//Serialize all children, recursively
		for (auto child : entity->GetTransform().GetChildren())
		{
			SaveChildOfPrefab(child->GetEntity(), serializer);
		}

		return true;
	}
	static ecs::EntityHandle LoadPrefab(std::string name)
	{
		PrefabManager* prefabManager = ST<PrefabManager>::Get();

		//Check if the prefab with this name exists
		if (prefabManager->_prefabPool.find(name) == prefabManager->_prefabPool.end())
		{
			CONSOLE_LOG(LEVEL_ERROR) << "INVALID PREFAB NAME: " << name;
			return nullptr;
		}
		ecs::POOL currentPool = ecs::GetCurrentPoolId();
		ecs::SwitchToPool(ecs::POOL::PREFAB_CACHE);

		ecs::EntityHandle clonedEntity = ecs::CloneEntityToPoolNow(prefabManager->_prefabPool[name], currentPool, true);

		ecs::SwitchToPool(currentPool);

		return clonedEntity;
	}
	void Update(std::string name = "")
	{
		if (!std::filesystem::exists(FolderDir()) && !std::filesystem::create_directory(FolderDir()))
		{
			CONSOLE_LOG(LEVEL_ERROR) << "Failed to create prefabs directory!";
			return;
		}

		_allPrefabs.resize(0);
		for (const auto& entry : std::filesystem::directory_iterator(FolderDir()))
		{
			_allPrefabs.push_back(entry.path().stem().string());
		}

		// Reload all prefabs in the pool
		// Get the current pool first so we can switch back to it after
		ecs::POOL initialPool = ecs::GetCurrentPoolId();
		ecs::SwitchToPool(ecs::POOL::PREFAB_CACHE);

		// If no name given, we reload ALL prefabs
		if (name == "")
		{
			// Remove all entities in the pool
			for (auto prefab : _prefabPool)
			{
				ecs::DeleteEntity(prefab.second);
			}

			// Reset the Prefab Pool map
			_prefabPool.erase(_prefabPool.begin(), _prefabPool.end());
			for (std::string prefabName : _allPrefabs)
			{
				ecs::EntityHandle prefabEntity = CreatePrefabEntityFromName(prefabName);
				if (prefabEntity == nullptr)
					continue;
				_prefabPool[prefabName] = prefabEntity;
			}
		}
		else
		{
			ecs::EntityHandle prefabEntity = nullptr;
			if (_prefabPool.find(name) != _prefabPool.end())
			{
				// Delete the prefab if it already exists
				prefabEntity = _prefabPool[name];
				if (prefabEntity != nullptr)
				{
					ecs::DeleteEntity(prefabEntity);
				}

			}
			// Attempt to load the prefab
			prefabEntity = CreatePrefabEntityFromName(name);

			// nullptr check, so if we delete a Prefab it is also possible to just call Update() for it
			if (prefabEntity != nullptr)
			{
				_prefabPool[name] = prefabEntity;
			}
			else
			{
				// When the entity doesn't exist, we can make sure to remove it from _prefabPool
				_prefabPool.erase(name);
				//_prefabPool.erase(std::find(_prefabPool.begin(), _prefabPool.end(), name));
			}
		}

		// Flush activeness setting
		ecs::FlushChanges();

		// Switch back to the previous pool
		ecs::SwitchToPool(initialPool);
	}
	const static std::vector<std::string>& AllPrefabs()
	{
		return ST<PrefabManager>::Get()->_allPrefabs;
	}
private:
	ecs::EntityHandle CreatePrefabEntityFromName(const std::string name)
	{
		Deserializer deserializer{ FolderDir() + "/" + name + ".prefab" };

		// Error Checking
		if (!deserializer.IsValid())
			return nullptr;

		ecs::EntityHandle parent{};
		while (deserializer.HasEntity())
		{
			ecs::EntityHandle e = ecs::CreateEntity();
			if (!parent)
				parent = e;
			deserializer.Deserialize(e);
		}
		return parent;
	}
};