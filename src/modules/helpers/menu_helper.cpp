#include "menu_helper.h"

void MenuHelper::drawMenu()
{
    std::vector<const char *> toDisplay;
    char selectedIndex = menuIndex;
    // If we're paginating, we need to be at the nearest multiple of 4
    char menuStartIndex = paginate ? (menuIndex / 4) * 4 : menuIndex;
    // Our menu should display the current menu item + 3 more.
    // If we don't have + 3 more to display, we work from the last - 3
    // If we don't have 4 total, just display the first x items.
    if (menuOptions.empty())
    {
        // We don't have anything to display. Fill it with empty stuff.
        toDisplay.push_back("");
        toDisplay.push_back("");
        toDisplay.push_back("");
        toDisplay.push_back("");
        selectedIndex = 0;
    }
    else if (menuOptions.size() <= 4)
    {
        // We don't have enough menu items to scroll Just print out all that we have
        for (const char *menuOption : menuOptions)
        {
            toDisplay.push_back(menuOption);
        }
    }
    else if (menuStartIndex + 4 > menuOptions.size() && !paginate)
    {
        // We don't have enough menu options after our selection. Grab the last four
        // menu items.
        for (auto it = (menuOptions.end() - 4); it != menuOptions.end(); it++)
        {
            toDisplay.push_back(*it);
        }
        selectedIndex = selectedIndex - (menuOptions.size() - 4);
        
    }
    else
    {
        // Grab the selected menu item and the three that follow
        for (auto it = (menuOptions.begin() + menuStartIndex); it != menuOptions.end(); it++)
        {
            toDisplay.push_back(*it);
        }
        selectedIndex = paginate ? (selectedIndex % 4) : 0;
        
    }

    // Fill in the rest of the list, if we weren't able to
    for (int i = toDisplay.size(); i < 4; i++)
    {
        toDisplay.push_back("");
    }

    // Print the current menu to the screen
    DisplayManager::drawFourOptionSelectScreen(menuTitle, toDisplay[0], toDisplay[1], toDisplay[2], toDisplay[3], selectedIndex, "^", "v");
}

void MenuHelper::moveUp()
{
    // Increment our menuIndex
    menuIndex++;

    // Loop around
    if (menuIndex >= menuOptions.size())
        menuIndex = 0;
}

void MenuHelper::moveDown()
{
    // Decrement our menuIndex
    menuIndex--;

    // Loop around
    if (menuIndex >= menuOptions.size())
        menuIndex = menuOptions.size() - 1;
}