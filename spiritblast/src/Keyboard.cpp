#include "Keyboard.h"

namespace Keyboard
{
	int GetOrdinal(string str)
	{
		return g_interface->CallBuiltin("ord", {RValue(str)}).ToInt32();
	}

	bool Check(int key)
	{
		return g_interface->CallBuiltin("keyboard_check", {key}).ToBoolean();
	}

	bool CheckPressed(int key)
	{
		return g_interface->CallBuiltin("keyboard_check_pressed", {key}).ToBoolean();
	}

	bool CheckReleased(int key)
	{
		return g_interface->CallBuiltin("keyboard_check_released", {key}).ToBoolean();
	}

	bool CheckDirect(int key)
	{
		return g_interface->CallBuiltin("keyboard_check_direct", {key}).ToBoolean();
	}

	bool Clear(int key)
	{
		return g_interface->CallBuiltin("keyboard_clear", {key}).ToBoolean();
	}

	void PressKey(int key)
	{
		g_interface->CallBuiltin("keyboard_key_press", {key});
	}

	void ReleaseKey(int key)
	{
		g_interface->CallBuiltin("keyboard_key_release", {key});
	}

	int GetKey()
	{
		RValue val = RValue();
		g_interface->GetBuiltin("keyboard_key", nullptr, NULL_INDEX, val);

		return val.ToInt32();
	}

	int GetLastKey()
	{
		RValue val = RValue();
		g_interface->GetBuiltin("keyboard_lastkey", nullptr, NULL_INDEX, val);

		return val.ToInt32();
	}

	const char *GetLastCharacter()
	{
		RValue val = RValue();
		g_interface->GetBuiltin("keyboard_lastchar", nullptr, NULL_INDEX, val);

		return val.ToCString();;
	}

	string GetKeyString()
	{
		RValue val = RValue();
		g_interface->GetBuiltin("keyboard_string", nullptr, NULL_INDEX, val);

		return val.ToString();
	}

	bool SetMap(int key, int key2)
	{
		return g_interface->CallBuiltin("keyboard_set_map", {key, key2}).ToBoolean();
	}

	int GetMap(int key)
	{
		return g_interface->CallBuiltin("keyboard_set_map", {key}).ToInt32();
	}

	void ResetMap()
	{
		g_interface->CallBuiltin("keyboard_unset_map", {});
	}

	void SetNumLock(bool enabled)
	{
		g_interface->CallBuiltin("keyboard_set_numlock", {enabled});
	}

	bool GetNumLock()
	{
		return g_interface->CallBuiltin("keyboard_get_numlock", {}).ToBoolean();
	}
}