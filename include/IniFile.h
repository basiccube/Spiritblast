#pragma once

#include "main.h"

// Opens an INI file located in the specified file path.
void IniOpen(string path);

// Opens an INI file from a string.
void IniOpenFromString(string ini);

// Closes the currently open INI file and returns a string containing its contents.
string IniClose();

// Writes a value to the current INI file.
void IniWrite(string section, string key, RValue value);

// Reads a value from the current INI file.
RValue IniRead(string section, string key, RValue defaultValue, bool isString = false);

// Returns whether or not a section exists in the current INI file.
bool IniSectionExists(string section);

// Returns whether or not a key within a section exists in the current INI file.
bool IniKeyExists(string section, string key);

// Deletes a section from the current INI file.
void IniDeleteSection(string section);

// Deletes a key from the current INI file.
void IniDeleteKey(string section, string key);

// Reads a value from the current INI file and sets the specified global variable.
RValue ReadAndSetGlobalFromIni(vector<RValue> iniArgs, string globalVar, bool isString = false);

// Gets the value of a global variable and writes it to the current INI file.
// NOTE: iniArgs should not contain the value and only the section and key.
RValue GetGlobalAndWriteToIni(vector<RValue> iniArgs, string globalVar);