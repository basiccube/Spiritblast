#include "main.h"

#include "RoomLoader.h"
#include <fstream>

json g_roomData;

json LoadRoomData(string path)
{
	ifstream file(path);
	if (file.fail())
	{
		Print("Failed to load room data");
		return nullptr;
	}

	json data;
	try
	{
		data = json::parse(file, nullptr, true, true);
	}
	catch (const json::parse_error &e)
	{
		Print("Failed to parse json file!");
		Print(e.what());
		return nullptr;
	}

	file.close();
	return data;
}

void GoToRoomLoaderRoom()
{
	g_roomData = LoadRoomData("testrm.json");
	if (g_roomData != nullptr)
	{
		RValue templateRoom = g_interface->CallBuiltin("asset_get_index", {RValue("rm_template_room")});
		if (templateRoom.ToInt32() != GM_INVALID)
		{
			Print("Preparing room");
			int width = g_roomData["roomInfo"]["width"];
			int height = g_roomData["roomInfo"]["height"];

			g_interface->CallBuiltin("room_set_width", {templateRoom, RValue(width)});
			g_interface->CallBuiltin("room_set_height", {templateRoom, RValue(height)});

			Print("Go to template room");
			g_interface->CallBuiltin("room_goto", {templateRoom});
		}
	}
}