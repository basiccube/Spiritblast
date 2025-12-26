#pragma once

#include <YYToolkit/YYTK_Shared.hpp>
using namespace Aurie;
using namespace YYTK;

#include <LuaCpp.hpp>

using namespace LuaCpp;
using namespace LuaCpp::Registry;
using namespace LuaCpp::Engine;

#include <string>
#include <vector>
#include <map>
using namespace std;

#include "Debug.h"

extern YYTKInterface *g_interface;
extern LuaContext *g_lua;

#define GM_C_WHITE RValue(16777215)
#define GM_C_BLACK RValue(0)

#define GM_INVALID -1

#define MOD_SETTINGS_FILE "spiritblast_settings.ini"

#define MOD_VERSION_MAJOR 0
#define MOD_VERSION_MINOR 4
#define MOD_VERSION_PATCH 0

inline string GetModVersion()
{
	return to_string(MOD_VERSION_MAJOR) + "." + to_string(MOD_VERSION_MINOR) + "." + to_string(MOD_VERSION_PATCH);
}