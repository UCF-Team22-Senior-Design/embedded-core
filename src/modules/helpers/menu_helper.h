#include <Arduino.h>
#include <vector>

#include "../../managers/display_manager.h"

class MenuHelper
{
    // Pass in an array of strings, navigate up, navigate down, set titles, easy stuff
private:
    const char* menuTitle; 
    std::vector<const char*> menuOptions;
    char menuIndex;
    bool paginate;
public:
    MenuHelper(const char* menuTitle, std::vector<const char*> menuOptions, char initialMenuIndex = 0, bool paginate = false)
     : menuTitle{ menuTitle }, menuOptions{ menuOptions }, menuIndex{ initialMenuIndex }, paginate{ paginate }  {};
    void drawMenu();
    void moveUp();
    void moveDown();
    void setIndex(char newIndex) { menuIndex = newIndex; }
    char getMenuIndex() { return menuIndex; };
};