#pragma once

#include "main.h"

inline void lprint(string str)
{
	Print(RValue("LUA: " + str));
}

void RegisterLuaInterfaceLibrary();