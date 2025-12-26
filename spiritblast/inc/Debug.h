#pragma once

#include "main.h"

extern bool g_debugOverlayOpen;

// Prints something to the log.
void Print(RValue str);

// Returns an array listing the current callstack.
vector<string> GetCallstack(int maxDepth = 24);

// Shows a popup window prompting the user to type in a number.
double ShowIntegerInputPopup(string str, double def);

// Shows a popup window prompting the user to type in a string.
string ShowStringInputPopup(string str, string def);

// Shows a crash window with the specified text.
void ShowError(string str);

// Shows a message box with the specified text.
void ShowMessage(string str);

// Shows a popup window asking the user a question with a "Yes" or "No" answer.
bool ShowQuestion(string str);

// Returns if the code is compiled, which will be true due to it being compiled with YYC.
bool IsCompiled();

// Gets the current FPS value.
int GetFPS();

// Gets the current real FPS value.
int GetRealFPS();

// Toggles the visibility of the debug overlay.
void ShowDebugOverlay(bool enable, bool fpsMinimized = true, float scale = 1.0f, float alpha = 0.8, bool gamepadEnabled = true, int gamepadIndex = -1);

// Toggles the log window from the debug overlay.
void ShowDebugOverlayLog(bool enable);

// Returns whether or not the mouse is over a debug overlay window.
bool IsMouseOverDebugOverlay();

// Returns whether or not the keyboard is currently in use by the debug overlay.
bool IsKeyboardUsedByDebugOverlay();