#include "ready_module.h"

const char *ReadyModule::STRING_MENU_TITLE = "Select Game Mode";
const char *ReadyModule::STRING_MENU_OPTION_FIRST = "Time Trial";
const char *ReadyModule::STRING_MENU_OPTION_SECOND = "One Shot";
const char *ReadyModule::STRING_MENU_OPTION_THIRD = "Wack-A-Mole";
const char *ReadyModule::STRING_MENU_OPTION_FOURTH = "Horde";
const char *ReadyModule::STRING_MENU_BUTTON_LEFT = "PAIR";
const char *ReadyModule::STRING_MENU_BUTTON_RIGHT = "PLAY";

MenuHelper ReadyModule::menuHelper(STRING_MENU_TITLE, {STRING_MENU_OPTION_FIRST, STRING_MENU_OPTION_SECOND, STRING_MENU_OPTION_THIRD, STRING_MENU_OPTION_FOURTH, "Pair", "Heheheh"}, 0, true);

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
    menuHelper.setIndex(0);

    Serial.println("I've woked!");
    menuHelper.drawMenu();

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

    /* Serial.printf("Ready Module Update [T: %lums, ∆T: %lums] | %d %d %d\n", time, deltaTime,
        InputManager::getInputState(InputSource::BUTTON_LEFT),
        InputManager::getInputState(InputSource::BUTTON_TRIGGER),
        InputManager::getInputState(InputSource::BUTTON_RIGHT));
     */
    static unsigned long lastButtonPress = 0;

    bool hasChanged = false;
    bool leftButtonPressed = InputManager::getInputState(InputSource::BUTTON_LEFT);
    bool rightButtonPressed = InputManager::getInputState(InputSource::BUTTON_RIGHT);

    // Decide upon left/right navigation
    if (leftButtonPressed && (time - lastButtonPress) > 250)
    {
        // Button is pressed - track time, increment value
        lastButtonPress = time;
        menuHelper.moveUp();
        hasChanged = true;
    }
    else if (rightButtonPressed && (time - lastButtonPress) > 250)
    {
        // Button is pressed - track time, increment value
        lastButtonPress = time;
        menuHelper.moveDown();
        hasChanged = true;
    }

    // Update display
    if(hasChanged)
        menuHelper.drawMenu();
}