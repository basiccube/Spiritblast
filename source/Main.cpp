#include "main.h"
#include "IniFile.h"
#include "Menus.h"
#include "RoomLoader.h"

YYTKInterface *g_interface = nullptr;

struct SpriteData
{
	RValue sprite;
	RValue x;
	RValue y;
	RValue xscale;
	RValue yscale;
	RValue angle;
};

vector<SpriteData> g_debugCollisionData;
vector<SpriteData> g_debugCollisionDataBG;
static bool g_showDebugCollision = false;

void Print(RValue str)
{
	g_interface->CallBuiltin("show_debug_message", {str});
}

void DrawDebugCollisionDataBG()
{
	RValue camID = RValue();
	g_interface->GetBuiltin("view_camera", nullptr, 0, camID);

	RValue camX = g_interface->CallBuiltin("camera_get_view_x", {camID});

	for (int i = 0; i < g_debugCollisionDataBG.size(); i++)
	{
		SpriteData &data = g_debugCollisionDataBG[i];
		g_interface->CallBuiltin("draw_sprite_ext", {data.sprite,
													RValue(0),
													RValue(data.x.ToInt32() + 25 + (camX.ToInt32() * 0.5)),
													data.y,
													data.xscale,
													data.yscale,
													data.angle,
													GM_C_WHITE,
													RValue(1)});
	}
}

void DrawDebugCollisionData()
{
	for (int i = 0; i < g_debugCollisionData.size(); i++)
	{
		SpriteData &data = g_debugCollisionData[i];
		g_interface->CallBuiltin("draw_sprite_ext", {data.sprite,
													RValue(0),
													data.x,
													data.y,
													data.xscale,
													data.yscale,
													data.angle,
													GM_C_WHITE,
													RValue(1)});
	}
}

void GetDebugCollisionData(const char* name, int gameplayLayer = 0)
{
	RValue obj = g_interface->CallBuiltin("asset_get_index", {RValue(name)});
	RValue instNum = g_interface->CallBuiltin("instance_number", {obj});

	for (int i = 0; i < instNum.ToInt32(); i++)
	{
		RValue inst = g_interface->CallBuiltin("instance_find", {obj, RValue(i)});
		if (inst.IsUndefined())
			continue;

		RValue obj = g_interface->CallBuiltin("variable_instance_get", {inst, RValue("object_index")});
		RValue objname = g_interface->CallBuiltin("object_get_name", {obj});
		RValue layer = g_interface->CallBuiltin("variable_instance_get", {inst, RValue("gameplayLayer")});

		if (objname.ToString() != name || layer.ToInt32() != gameplayLayer)
			continue;

		SpriteData data = {
			g_interface->CallBuiltin("variable_instance_get", {inst, RValue("sprite_index")}),
			g_interface->CallBuiltin("variable_instance_get", {inst, RValue("x")}),
			g_interface->CallBuiltin("variable_instance_get", {inst, RValue("y")}),
			g_interface->CallBuiltin("variable_instance_get", {inst, RValue("image_xscale")}),
			g_interface->CallBuiltin("variable_instance_get", {inst, RValue("image_yscale")}),
			g_interface->CallBuiltin("variable_instance_get", {inst, RValue("image_angle")})
		};

		RValue validSprite = g_interface->CallBuiltin("sprite_exists", {data.sprite});
		if (!validSprite.ToBoolean())
		{
			g_interface->CallBuiltin("show_debug_message", {data.sprite});
			Print("Not a valid sprite, ignoring...");
			continue;
		}

		if (gameplayLayer > 0)
			g_debugCollisionDataBG.push_back(data);
		else
			g_debugCollisionData.push_back(data);
	}
}

RValue GetInstance(string name)
{
	RValue obj = g_interface->CallBuiltin("asset_get_index", {RValue(name)});
	RValue instNum = g_interface->CallBuiltin("instance_number", {obj});

	for (int i = 0; i < instNum.ToInt32(); i++)
	{
		RValue inst = g_interface->CallBuiltin("instance_find", {obj, RValue(i)});
		if (!inst.IsUndefined())
			return inst;
	}
	
	return RValue();
}

bool DumpVariableFunc(const char *name, RValue *value)
{
	Print(name);
	return false;
}

void DumpGlobalVariables()
{
	Print("Dumping global variables...");

	CInstance *globalInst{};
	AurieStatus gres = g_interface->GetGlobalInstance(&globalInst);
	if (AurieSuccess(gres))
		g_interface->EnumInstanceMembers(globalInst, DumpVariableFunc);
}

void DumpObjectVariables(string name)
{
	Print(RValue("Dumping object variables: " + name));

	RValue inst = GetInstance(name);
	if (inst.IsUndefined())
	{
		Print(RValue("No instance found for object " + name));
		return;
	}	
	
	RValue arr = g_interface->CallBuiltin("variable_instance_get_names", {inst});

	vector<RValue> nameArr = arr.ToVector();
	for (int j = 0; j < nameArr.size(); j++)
		g_interface->CallBuiltin("show_debug_message", {nameArr[j]});
}

static bool g_createdDebugMenu = false;
static bool g_createdCustomOptions = false;

static bool g_debugControls = false;
static bool g_skipSplash = false;
static int g_debugOptionsIndex = -1;

// Hook used for changing the header text in the options menu.
RValue &EnvironmentGetUsernameHook(CInstance *self, CInstance *other, RValue &returnValue, int argCount, RValue **args)
{
	RValue selfValue = self->ToRValue();
	RValue otherValue = other->ToRValue();

	if (otherValue.IsStruct())
	{
		RValue displayExists = g_interface->CallBuiltin("struct_exists", {otherValue, "displayPage"});
		if (displayExists.ToBoolean())
		{
			RValue pages = g_interface->CallBuiltin("struct_get", {otherValue, "pages"});
			RValue currentPage = g_interface->CallBuiltin("struct_get", {otherValue, "currentPage"});
			RValue currentIndex = g_interface->CallBuiltin("array_get_index", {pages, currentPage});

			if (currentIndex.ToInt32() == g_debugOptionsIndex)
				returnValue = "Debug";
			else
			{
				RValue getPageHeader = g_interface->CallBuiltin("struct_get", {otherValue, "getPageHeaderOriginal"});
				RValue headerValue = g_interface->CallBuiltin("method_call", {getPageHeader});
				returnValue = headerValue;
			}
		}
		else
			goto environment_get_username_func;
	}
	else
		goto environment_get_username_func;

	return returnValue;

environment_get_username_func:
	RValue username = g_interface->CallBuiltin("environment_get_variable", {"USERNAME"});
	returnValue = username;
	return returnValue;
}

void FrameCallback(FWFrame &frameCtx)
{
	UNREFERENCED_PARAMETER(frameCtx);

	if (!g_debugControls)
		return;

	RValue dumpGlobalPress = g_interface->CallBuiltin("keyboard_check_pressed", {RValue(VK_F1)});
	if (dumpGlobalPress.ToBoolean())
		DumpGlobalVariables();

	RValue dumpObjectPress = g_interface->CallBuiltin("keyboard_check_pressed", {RValue(VK_F2)});
	if (dumpObjectPress.ToBoolean())
	{
		RValue inputString = g_interface->CallBuiltin("get_string", {RValue("Type the name of the object you want to dump all of the variables from."), RValue("ob_player")});
		if (inputString.ToString() != "")
			DumpObjectVariables(inputString.ToString());
	}

	RValue gotoTemplateRoom = g_interface->CallBuiltin("keyboard_check_pressed", {RValue(VK_F3)});
	if (gotoTemplateRoom.ToBoolean())
		GoToRoomLoaderRoom();

	bool prevDebugCollision = g_showDebugCollision;
	RValue colKeyPress = g_interface->CallBuiltin("keyboard_check_pressed", {RValue(VK_F4)});
	if (colKeyPress.ToBoolean())
		g_showDebugCollision = !g_showDebugCollision;

	RValue changePagePress = g_interface->CallBuiltin("keyboard_check_pressed", {RValue(VK_F5)});
	if (changePagePress.ToBoolean())
	{
		RValue inputString = g_interface->CallBuiltin("get_string", {RValue("Type the variable name of the page you want to go to."), RValue("mainPage")});
		if (inputString.ToString() != "")
		{
			RValue menuInst = GetInstance("ob_listMenu");
			if (menuInst.IsUndefined())
			{
				Print("Not in a menu!");
				return;
			}

			ChangeMenuPage(menuInst, inputString.ToString());
		}
	}
}

void EventCallback(FWCodeEvent &eventCtx)
{
	RValue event_type = RValue();
	RValue event_number = RValue();
	RValue event_object = RValue();
	RValue event_object_name = RValue();

	g_interface->GetBuiltin("event_type", nullptr, NULL_INDEX, event_type);
	g_interface->GetBuiltin("event_number", nullptr, NULL_INDEX, event_number);
	g_interface->GetBuiltin("event_object", nullptr, NULL_INDEX, event_object);
	event_object_name = g_interface->CallBuiltin("object_get_name", {event_object});

	switch (event_type.ToInt32())
	{
		// Create event
		case 0:
			break;

		// Destroy event
		case 1:
			break;

		// Step type
		case 3:
			switch (event_number.ToInt32())
			{
				// Step event
				case 0:
					break;

				// End Step event
				case 2:
					// Create the debug menu and button.
					// This has to happen here and not in the create event above
					// because this needs the variables that are created in the menus create event
					// and the custom code is executed right before the event.
					if (event_object_name.ToString() == "ob_mainMenu" && !g_createdDebugMenu)
					{
						RValue menu = GetInstance("ob_mainMenu");
						if (menu.IsUndefined())
						{
							Print("No main menu instance found");
							break;
						}

						/*
						RValue debugPage = CreateMenuPage(menu);
						{
							RValue mouseEnabledValue = g_interface->CallBuiltin("variable_instance_get", {menu, "mouseEnabled"});
							CInstance *mouseToggle = CreateMenuToggle(menu, "Mouse Enabled", mouseEnabledValue, menu, "mouseEnabled");
							AddItemToPageValue(menu, debugPage, mouseToggle);

							CInstance *backBtn = CreateBackButton(menu, "Back");
							AddItemToPageValue(menu, debugPage, backBtn);
						}

						CInstance *debugMenuBtn = CreateChangePageButton(menu, "Debug", debugPage);
						AddItemToPage(menu, "mainPage", debugMenuBtn);
						*/

						g_createdDebugMenu = true;
					}
					break;
			}
			break;

		// Other type
		case 7:
			switch (event_number.ToInt32())
			{
				// Room start event
				case 4:
					if (event_object_name.ToString() == "ob_camera")
					{
						RValue room = RValue();
						g_interface->GetBuiltin("room", nullptr, NULL_INDEX, room);
						RValue roomName = g_interface->CallBuiltin("room_get_name", {room});

						// Load the custom room data if we are in the template room
						if (roomName.ToString() == "rm_template_room")
						{
							RValue playerObject = g_interface->CallBuiltin("asset_get_index", {RValue("ob_player")});
							if (!g_interface->CallBuiltin("instance_exists", {playerObject}).ToBoolean())
							{
								Print("Creating player object");
								g_interface->CallBuiltin("instance_create_depth", {RValue(32), RValue(32), RValue(0), playerObject});
							}

							vector<json> objectData = g_roomData["objects"];
							for (int i = 0; i < objectData.size(); i++)
							{
								json obj = objectData[i];

								string oid = obj["id"];
								int ox = obj["x"];
								int oy = obj["y"];
								int oxscale = obj["xscale"];
								int oyscale = obj["yscale"];

								RValue oasset = g_interface->CallBuiltin("asset_get_index", {RValue(oid)});
								if (oasset.ToInt32() == GM_INVALID)
									continue;

								RValue oinst = g_interface->CallBuiltin("instance_create_layer", {RValue(ox), RValue(oy), RValue("InstancesFG"), oasset});
								g_interface->CallBuiltin("variable_instance_set", {oinst, RValue("image_xscale"), RValue(oxscale)});
								g_interface->CallBuiltin("variable_instance_set", {oinst, RValue("image_yscale"), RValue(oyscale)});
							}
						}
						else if (roomName.ToString() == "rm_splashScreen" && g_skipSplash)
						{
							RValue rm_logoDrop = g_interface->CallBuiltin("asset_get_index", {"rm_logoDrop"});
							if (rm_logoDrop.ToInt32() != GM_INVALID)
							{
								Print("Skip splash screens");
								g_interface->CallBuiltin("room_goto", {rm_logoDrop});
							}
						}

						// Get the sprite data from all of the debug collision objects
						Print("Getting debug collision sprite data");

						const char *colObjects[] = {"ob_block", "ob_passthrough", "ob_passthroughSlope14",
													"ob_passthroughSlope22", "ob_passthroughSlope45",
													"ob_slope14", "ob_slope22", "ob_slope45", "ob_ladder"};

						for (int i = 0; i < sizeof(colObjects) / sizeof(colObjects[0]); i++)
							GetDebugCollisionData(colObjects[i]);

						const char *colObjectsBG[] = {"ob_block_BG", "ob_passthrough_BG", "ob_ladder_BG",
													"ob_passthroughSlope22_BG", "ob_passthroughSlope45_BG",
													"ob_slope12_BG", "ob_slope22_BG", "ob_slope45_BG"};

						for (int i = 0; i < sizeof(colObjectsBG) / sizeof(colObjectsBG[0]); i++)
							GetDebugCollisionData(colObjectsBG[i], 1);
					}
					break;

				// Room end event
				case 5:
					if (event_object_name.ToString() == "ob_camera")
					{
						Print("Clearing debug collision sprite data");
						g_debugCollisionData.clear();
						g_debugCollisionDataBG.clear();
					}

					g_createdDebugMenu = false;
					g_createdCustomOptions = false;
					break;
			}
			break;

		// Draw type
		case 8:
			switch (event_number.ToInt32())
			{
				// Draw event
				case 0:
					if (g_showDebugCollision && event_object_name.ToString() == "ob_particleBGDrawer")
						DrawDebugCollisionDataBG();
					if (g_showDebugCollision && event_object_name.ToString() == "ob_camera")
						DrawDebugCollisionData();

					// Create all of the custom options.
					if (event_object_name.ToString() == "ob_optionsMenu")
					{
						if (!g_createdCustomOptions)
						{
							RValue menu = GetInstance("ob_optionsMenu");
							if (menu.IsUndefined())
							{
								Print("No options menu instance found");
								break;
							}

							CInstance *globalInst{};
							AurieStatus globalResult = g_interface->GetGlobalInstance(&globalInst);

							if (!AurieSuccess(globalResult))
								break;

							RValue debugPage = CreateMenuPage(menu);
							{
								RValue debugOverlayValue = g_interface->CallBuiltin("variable_global_get", {"debug_overlay"});
								CInstance *debugOverlayInst = CreateMenuToggle(menu, "Debug Overlay", debugOverlayValue, globalInst, "debug_overlay");
								AddItemToPageValue(menu, debugPage, debugOverlayInst);

								RValue debugControlsValue = g_interface->CallBuiltin("variable_global_get", {"debug_controls"});
								CInstance *debugControlsInst = CreateMenuToggle(menu, "Debug Controls", debugControlsValue, globalInst, "debug_controls");
								AddItemToPageValue(menu, debugPage, debugControlsInst);

								RValue skipSplashValue = g_interface->CallBuiltin("variable_global_get", {"skip_splash"});
								CInstance *skipSplashInst = CreateMenuToggle(menu, "Skip Splash Screens", skipSplashValue, globalInst, "skip_splash");
								AddItemToPageValue(menu, debugPage, skipSplashInst);

								CInstance *backBtn = CreateBackButton(menu, "Back");
								AddItemToPageValue(menu, debugPage, backBtn);
							}

							RValue pages = g_interface->CallBuiltin("variable_instance_get", {menu, "pages"});
							RValue debugPageIndex = g_interface->CallBuiltin("array_get_index", {pages, debugPage});
							g_debugOptionsIndex = debugPageIndex.ToInt32();

							RValue getPageHeader = g_interface->CallBuiltin("variable_instance_get", {menu, "getPageHeader"});
							g_interface->CallBuiltin("variable_instance_set", {menu, "getPageHeaderOriginal", getPageHeader});

							RValue environment_get_username = g_interface->CallBuiltin("variable_global_get", {"environment_get_username"});
							g_interface->CallBuiltin("variable_instance_set", {menu, "getPageHeader", environment_get_username});

							CInstance *debugPageBtn = CreateChangePageButton(menu, "Debug", debugPage);
							AddItemToPage(menu, "mainPage", debugPageBtn, 5);

							// For some reason creating a page in the options menu also changes the current page to it
							ChangeMenuPage(menu, "mainPage", false);

							RValue bfullscreenValue = g_interface->CallBuiltin("variable_global_get", {"borderless_fullscreen"});
							CInstance *bfullscreenInst = CreateMenuToggle(menu, "Borderless Fullscreen", bfullscreenValue, globalInst, "borderless_fullscreen");
							AddItemToPage(menu, "displayPage", bfullscreenInst, 2);

							g_createdCustomOptions = true;
						}
						
						RValue prevBorderlessValue = g_interface->CallBuiltin("window_get_borderless_fullscreen", {});
						RValue borderlessValue = g_interface->CallBuiltin("variable_global_get", {"borderless_fullscreen"});

						if (prevBorderlessValue.ToBoolean() != borderlessValue.ToBoolean())
						{
							Print("Borderless fullscreen value has changed, setting borderless fullscreen mode");
							g_interface->CallBuiltin("window_enable_borderless_fullscreen", {borderlessValue});
						}
					}
					break;

				// Draw Begin event
				case 72:
					break;
			}
			break;

		// Clean Up event
		case 12:
			if (event_object_name.ToString() == "ob_listMenu")
			{
				g_createdDebugMenu = false;
				g_createdCustomOptions = false;

				// Write any custom settings to the ini file
				IniOpen(MOD_SETTINGS_FILE);

				GetGlobalAndWriteToIni({"Video", "BorderlessFullscreen"}, "borderless_fullscreen");

				RValue debugOverlayValue = GetGlobalAndWriteToIni({"Debug", "DebugOverlay"}, "debug_overlay");
				g_interface->CallBuiltin("show_debug_overlay", {debugOverlayValue});

				RValue debugControlsValue = GetGlobalAndWriteToIni({"Debug", "DebugControls"}, "debug_controls");
				g_debugControls = debugControlsValue.ToBoolean();

				RValue skipSplashValue = GetGlobalAndWriteToIni({"Debug", "SkipSplash"}, "skip_splash");
				g_skipSplash = skipSplashValue.ToBoolean();

				IniClose();
			}
			break;
	}
}

EXPORTED AurieStatus ModuleInitialize(IN AurieModule* Module, IN const fs::path& ModulePath)
{
	UNREFERENCED_PARAMETER(ModulePath);

	AurieStatus status = AURIE_SUCCESS;

	g_interface = YYTK::GetInterface();
	if (!g_interface)
		return AURIE_MODULE_DEPENDENCY_NOT_RESOLVED;
	
	status = g_interface->CreateCallback(Module, EVENT_FRAME, FrameCallback, 2);
	if (!AurieSuccess(status))
		printf("Failed to create frame callback\n");

	status = g_interface->CreateCallback(Module, EVENT_OBJECT_CALL, EventCallback, 1);
	if (!AurieSuccess(status))
		printf("Failed to create event callback\n");

	Print("Mod successfully initialized!");

	// I don't know if this does anything
	//g_interface->CallBuiltin("variable_global_set", {RValue("debug"), RValue(true)});
	//g_interface->CallBuiltin("variable_global_set", {RValue("debug_visible"), RValue(true)});

	// Read all custom settings from the settings INI.
	IniOpen(MOD_SETTINGS_FILE);

	// Borderless fullscreen
	RValue bfullscreenValue = ReadAndSetGlobalFromIni({"Video", "BorderlessFullscreen", false}, "borderless_fullscreen");
	g_interface->CallBuiltin("window_enable_borderless_fullscreen", {bfullscreenValue});

	// Debug overlay
	RValue debugOverlayValue = ReadAndSetGlobalFromIni({"Debug", "DebugOverlay", false}, "debug_overlay");
	g_interface->CallBuiltin("show_debug_overlay", {debugOverlayValue});

	// Debug controls
	RValue debugControlsValue = ReadAndSetGlobalFromIni({"Debug", "DebugControls", false}, "debug_controls");
	g_debugControls = debugControlsValue.ToBoolean();

	// Skip splash screens
	RValue skipSplashValue = ReadAndSetGlobalFromIni({"Debug", "SkipSplash", false}, "skip_splash");
	g_skipSplash = skipSplashValue.ToBoolean();

	IniClose();

	// Hook into environment_get_username to override the options menu header later
	// I'm pretty certain this function is not used in the game at all so hooking into it is fine
	CScript *scrData = nullptr;
	TRoutine scrFunction = nullptr;

	status = g_interface->GetNamedRoutinePointer("gml_Script_environment_get_username", reinterpret_cast<PVOID*>(&scrData));
	if (!AurieSuccess(status))
		Print("Failed to get function data");
	else
	{
		Print("Hooking into environment_get_username...");
		MmCreateHook(Module, "environment_get_username_hook", scrData->m_Functions->m_ScriptFunction, EnvironmentGetUsernameHook, reinterpret_cast<PVOID*>(&scrFunction));
	}

	return AURIE_SUCCESS;
}