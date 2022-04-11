#include "module.h"

#include "../managers/display_manager.h"
#include "helpers/menu_helper.h"

enum ReadyModuleMenuState
{
    MAIN,
    SETTINGS,
    GAME
};

class ReadyModule : public Module
{
private:
    static Task moduleTask;
    // Constant, private variables
    static const char* STRING_MENU_TITLE;
    static const char* STRING_MENU_OPTION_FIRST;
    static const char* STRING_MENU_OPTION_SECOND;
    static const char* STRING_MENU_OPTION_THIRD;
    static const char* STRING_MENU_OPTION_FOURTH;
    static const char* STRING_MENU_OPTION_FIFTH;
    static const char* STRING_MENU_OPTION_SIXTH;
    static const char* STRING_MENU_BUTTON_LEFT;
    static const char* STRING_MENU_BUTTON_RIGHT;

    static const char* STRING_SETTINGS_MENU_OPTION_FIRST;
    static const char* STRING_SETTINGS_MENU_TITLE;
    static const char* STRING_SETTINGS_MENU_OPTION_SECOND;
    static const char* STRING_SETTINGS_MENU_OPTION_THIRD;
    static const char* STRING_SETTINGS_MENU_OPTION_FOURTH;
    static const char* STRING_SETTINGS_MENU_OPTION_FIFTH;
    static const char* STRING_SETTINGS_MENU_OPTION_SIXTH;
    static const char* STRING_SETTINGS_MENU_OPTION_SEVENTH;
    static const char* STRING_SETTINGS_MENU_OPTION_EIGHTH;
    static const char* STRING_SETTINGS_MENU_OPTION_NINTH;
    static const char* STRING_SETTINGS_MENU_OPTION_TENTH;
    static const char* STRING_SETTINGS_MENU_OPTION_ELEVENTH;

    static const char* STRING_GAME_MENU_TITLE;
    static const char* STRING_GAME_MENU_OPTION_FIRST;
    static const char* STRING_GAME_MENU_OPTION_SECOND;
    static const char* STRING_GAME_MENU_OPTION_THIRD;
    static const char* STRING_GAME_MENU_OPTION_FOURTH;
    static const char* STRING_GAME_MENU_OPTION_FIFTH;

    static MenuHelper mainMenuHelper;
    static MenuHelper settingsMenuHelper;
    static MenuHelper gameModeMenuHelper;

    static ReadyModuleMenuState menuIndex;

    static bool handleMenuSelection();
    static bool handleMainMenuSelection();
    static bool handleSettingMenuSelection();
    static bool handleGameMenuSelection();
    
    static void handleTriggerPull(InputSource source, bool state);
    static void handleMenuLeft(InputSource _, bool state);
    static void handleMenuRight(InputSource _, bool state);

    static void refreshDisplay();
public:
    static void initialize(Scheduler *userScheduler);
    static bool onWake();
    static void onSleep();
    static void onUpdate();

    static Task* getTask() { return &moduleTask; };
};