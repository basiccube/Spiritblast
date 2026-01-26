#pragma once

#include "main.h"

#include <string>
#include <map>
#include <vector>

#include <json.hpp>
using json = nlohmann::json;

#define ROOM_VERSION 1

namespace RoomLoader
{
	extern json roomData;
	extern string currentRoom;
	extern string firstRoom;
	extern string levelName;

	extern map<string, vector<string>> instanceMap;

	// Returns the string used for the instance map.
	string GetInstanceIDString(string obj, float x, float y);

	// Clears all room instance lists in the instance map.
	void ClearInstanceMap();

	// Creates stage data for a level.
	map<string, RValue> CreateStageData();

	// Loads room data from the specified file path.
	json LoadRoomData(string path);

	// Goes to a room from the specified level.
	void GoToRoom(string level, string name);

	// Initializes the room using the currently loaded room data.
	void InitializeRoom();

	// Creates required hooks for the room loader.
	void InitializeLoaderHooks();
};