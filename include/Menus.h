#pragma once

#include "main.h"

// Changes the current page in the menu.
// NOTE: addToHistory is meant to be used as a boolean if specified.
void ChangeMenuPage(RValue menu, string name, int addToHistory = -1);

// Creates a menu page.
RValue CreateMenuPage(RValue menu);

// Adds a menu item to the page stored in an RValue.
void AddItemToPageValue(RValue menu, RValue page, CInstance *item, int index = -1);

// Adds a menu item to a page.
void AddItemToPage(RValue menu, string page, CInstance *item, int index = -1);

// Creates a menu button.
CInstance *CreateMenuButton(RValue menu, string text, RValue buttonMethod, RValue params = RValue());

// Creates a menu on/off toggle.
CInstance *CreateMenuToggle(RValue menu, string text, RValue value, RValue object, RValue varName);

// Creates a menu button that goes to the specified page.
CInstance *CreateChangePageButton(RValue menu, string text, RValue targetPage);

// Creates a menu button that goes back to the previous page.
CInstance *CreateBackButton(RValue menu, string text);