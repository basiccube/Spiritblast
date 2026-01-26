#pragma once

#include "main.h"

namespace Keyboard
{
	// Returns the Unicode value for a character.
	int GetOrdinal(string str);

	// Returns whether a key is being held down or not.
	bool Check(int key);

	// Returns whether a key has been pressed or not.
	bool CheckPressed(int key);

	// Returns whether a key has been released or not.
	bool CheckReleased(int key);

	// Returns whether a key is being pressed by checking the hardware directly.
	bool CheckDirect(int key);

	// Clears the state of the specified key.
	bool Clear(int key);

	// Simulates a key press.
	void PressKey(int key);

	// Simulates a key release.
	void ReleaseKey(int key);

	// Gets the key that's currently being pressed.
	int GetKey();

	// Gets the key that was pressed in the previous frame.
	int GetLastKey();

	// Gets the character of the last key being pressed.
	const char *GetLastCharacter();

	// Returns a string of the last 1024 keys typed on the keyboard.
	string GetKeyString();

	// Maps a key to another one.
	bool SetMap(int key, int key2);

	// Returns the ASCII code for a mapped key.
	int GetMap(int key);

	// Resets any remapped keys to normal.
	void ResetMap();
	
	// Sets number-lock on or off.
	void SetNumLock(bool enabled);

	// Returns the state of number-lock.
	bool GetNumLock();
}