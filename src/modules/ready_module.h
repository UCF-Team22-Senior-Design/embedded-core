#include "module.h"

#include "../managers/display_manager.h"
#include "helpers/menu_helper.h"

class ReadyModule : public Module
{
private:
    // Constant, private variables
    static const char* STRING_MENU_TITLE;
    static const char* STRING_MENU_OPTION_FIRST;
    static const char* STRING_MENU_OPTION_SECOND;
    static const char* STRING_MENU_OPTION_THIRD;
    static const char* STRING_MENU_OPTION_FOURTH;
    static const char* STRING_MENU_BUTTON_LEFT;
    static const char* STRING_MENU_BUTTON_RIGHT;

    static MenuHelper menuHelper;
public:
    static void initialize(Scheduler *userScheduler);
    static bool onWake();
    static void onSleep();
    static void onUpdate();
};