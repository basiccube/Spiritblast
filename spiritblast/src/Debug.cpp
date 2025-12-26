#include "Debug.h"

bool g_debugOverlayOpen = false;

void Print(RValue str)
{
	g_interface->CallBuiltin("show_debug_message", {str});
}

vector<string> GetCallstack(int maxDepth)
{
	RValue callstack = g_interface->CallBuiltin("debug_get_callstack", {maxDepth});

	vector<RValue> stackVector = callstack.ToVector();
	vector<string> stringVector;
	for (RValue val : stackVector)
	{
		string str = val.ToString();
		stringVector.push_back(str);
	}

	return stringVector;
}

double ShowIntegerInputPopup(string str, double def)
{
	RValue val = g_interface->CallBuiltin("get_integer", {RValue(str), def});
	return val.ToDouble();
}

string ShowStringInputPopup(string str, string def)
{
	RValue val = g_interface->CallBuiltin("get_string", {RValue(str), RValue(def)});
	return val.ToString();
}

void ShowError(string str)
{
	g_interface->CallBuiltin("show_error", {RValue(str), true});
}

void ShowMessage(string str)
{
	g_interface->CallBuiltin("show_message", {RValue(str)});
}

bool ShowQuestion(string str)
{
	RValue val = g_interface->CallBuiltin("show_question", {RValue(str)});
	return val.ToBoolean();
}

bool IsCompiled()
{
	RValue val = g_interface->CallBuiltin("code_is_compiled", {});
	return val.ToBoolean();
}

int GetFPS()
{
	RValue val;
	g_interface->GetBuiltin("fps", nullptr, NULL_INDEX, val);

	return val.ToInt32();
}

int GetRealFPS()
{
	RValue val;
	g_interface->GetBuiltin("fps_real", nullptr, NULL_INDEX, val);

	return val.ToInt32();
}

void ShowDebugOverlay(bool enable, bool fpsMinimized, float scale, float alpha, bool gamepadEnabled, int gamepadIndex)
{
	g_interface->CallBuiltin("show_debug_overlay", {enable, fpsMinimized, scale, alpha, gamepadEnabled, gamepadIndex});

	RValue open = g_interface->CallBuiltin("is_debug_overlay_open", {});
	g_debugOverlayOpen = open.ToBoolean();
}

void ShowDebugOverlayLog(bool enable)
{
	g_interface->CallBuiltin("show_debug_log", {enable});
}

bool IsMouseOverDebugOverlay()
{
	RValue val = g_interface->CallBuiltin("is_mouse_over_debug_overlay", {});
	return val.ToBoolean();
}

bool IsKeyboardUsedByDebugOverlay()
{
	RValue val = g_interface->CallBuiltin("is_keyboard_used_debug_overlay", {});
	return val.ToBoolean();
}