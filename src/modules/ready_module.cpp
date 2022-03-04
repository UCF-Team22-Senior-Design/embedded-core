#include "ready_module.h"

const char *ReadyModule::STRING_MENU_TITLE = "Select Game Mode";
const char *ReadyModule::STRING_MENU_OPTION_FIRST = "Time Trial";
const char *ReadyModule::STRING_MENU_OPTION_SECOND = "One Shot";
const char *ReadyModule::STRING_MENU_OPTION_THIRD = "Wack-A-Mole";
const char *ReadyModule::STRING_MENU_OPTION_FOURTH = "Horde";
const char *ReadyModule::STRING_MENU_BUTTON_LEFT = "PAIR";
const char *ReadyModule::STRING_MENU_BUTTON_RIGHT = "PLAY";

int ReadyModule::currentMenuPage = 0;

/**
 * @brief Establish our task, bind it to the scheduler.
 *
 * @param userScheduler
 */
void ReadyModule::initialize(Scheduler *userScheduler)
{
    moduleTask.set(TASK_MILLISECOND * 16, TASK_FOREVER, &onUpdate, &onWake, &onSleep);

    // Bind it to our scheduler
    (*userScheduler).addTask(moduleTask);
}

/**
 * @brief This function is called when the readyModule task is enabled. Use it to
 *        prepare variables (load from configuration data), reset operating things,
 *        and whatever else you'd like. Must return true, otherwise the task will
 *        not wake up and the system will be dead.
 */
bool ReadyModule::onWake()
{
    // Reset our menu page to zero;
    currentMenuPage = 1;

    Serial.println("I've woked!");
    refreshMenu();

    return true;
}

/**
 * @brief This function is called when the readyModule task is disabled. Use it
 *        to store changes, reset variables, etc.
 */
void ReadyModule::onSleep()
{
    // Put to sleep whatever we have been running
    Serial.println("I sleep");
}

/**
 * @brief Runs every "frame" - perform fixed update things, such as checking
 *        inputs, updating screen, etc.
 */
void ReadyModule::onUpdate()
{
    static unsigned long lastMillis = millis();
    unsigned long time = millis();
    unsigned long deltaTime = time - lastMillis;
    lastMillis = time;

    // Serial.printf("Ready Module Update [∆T: %lums] ", deltaTime);
    static unsigned long lastButtonPressLeft = 0;
    static unsigned long lastButtonPressRight = 0;

    int lastMenuPage = currentMenuPage;
    bool leftButtonPressed = !InputManager::getInputState(InputSource::BUTTON_LEFT);
    bool rightButtonPressed = !InputManager::getInputState(InputSource::BUTTON_RIGHT);

    // Decide upon left/right navigation
    if (leftButtonPressed && (time - lastButtonPressLeft) > 250)
    {
        // Button is pressed - track time, increment value
        lastButtonPressLeft = time;
        currentMenuPage--;
    }
    else if (rightButtonPressed && (time - lastButtonPressRight) > 250)
    {
        // Button is pressed - track time, increment value
        lastButtonPressRight = time;
        currentMenuPage++;
    }

    // Wrap selection back around
    if (currentMenuPage > 4)
    {
        currentMenuPage = 1;
    }
    else if (currentMenuPage < 1)
    {
        currentMenuPage = 4;
    }

    // Update display
    if (lastMenuPage != currentMenuPage)
        refreshMenu();
}

/**
 * @brief Redraws the current menu page with a call to the display manager.
 *  Do not call this every update, as it will cause the screen to flicker -
 *  it is best to only draw every time something changes.
 */
void ReadyModule::refreshMenu()
{
    // Call the display manager function
    DisplayManager::drawFourOptionSelectScreen(STRING_MENU_TITLE, STRING_MENU_OPTION_FIRST, STRING_MENU_OPTION_SECOND, STRING_MENU_OPTION_THIRD, STRING_MENU_OPTION_FOURTH, currentMenuPage, STRING_MENU_BUTTON_LEFT, STRING_MENU_BUTTON_RIGHT);
}