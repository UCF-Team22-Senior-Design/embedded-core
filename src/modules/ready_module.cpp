#include "ready_module.h"

const char *ReadyModule::STRING_MENU_TITLE = "Main Menu";
const char *ReadyModule::STRING_MENU_OPTION_FIRST = "Select Game Mode";
const char *ReadyModule::STRING_MENU_OPTION_SECOND = "Pair Targets";
const char *ReadyModule::STRING_MENU_OPTION_THIRD = "Change Settings";
const char *ReadyModule::STRING_MENU_OPTION_FOURTH = "About";

const char *ReadyModule::STRING_SETTINGS_MENU_TITLE = "Settings";
const char *ReadyModule::STRING_SETTINGS_MENU_OPTION_FIRST = "Mute Audio";
const char *ReadyModule::STRING_SETTINGS_MENU_OPTION_SECOND = "25%% Volume";
const char *ReadyModule::STRING_SETTINGS_MENU_OPTION_THIRD = "50%% Volume";
const char *ReadyModule::STRING_SETTINGS_MENU_OPTION_FOURTH = "75%% Volume";
const char *ReadyModule::STRING_SETTINGS_MENU_OPTION_FIFTH = "100%% Volume";
const char *ReadyModule::STRING_SETTINGS_MENU_OPTION_SIXTH = "Reset Settings";
const char *ReadyModule::STRING_SETTINGS_MENU_OPTION_SEVENTH = "Back";

const char *ReadyModule::STRING_GAME_MENU_TITLE = "Select Game Mode";
const char *ReadyModule::STRING_GAME_MENU_OPTION_FIRST = "One-Shot";
const char *ReadyModule::STRING_GAME_MENU_OPTION_SECOND = "Wack-A-Mole";
const char *ReadyModule::STRING_GAME_MENU_OPTION_THIRD = "Horde";
const char *ReadyModule::STRING_GAME_MENU_OPTION_FOURTH = "Time Trial";
const char *ReadyModule::STRING_GAME_MENU_OPTION_FIFTH = "Back";

MenuHelper ReadyModule::mainMenuHelper(STRING_MENU_TITLE, {STRING_MENU_OPTION_FIRST, STRING_MENU_OPTION_SECOND, STRING_MENU_OPTION_THIRD, STRING_MENU_OPTION_FOURTH});
MenuHelper ReadyModule::settingsMenuHelper(STRING_SETTINGS_MENU_TITLE, {STRING_SETTINGS_MENU_OPTION_FIRST, STRING_SETTINGS_MENU_OPTION_SECOND, STRING_SETTINGS_MENU_OPTION_THIRD, STRING_SETTINGS_MENU_OPTION_FOURTH, STRING_SETTINGS_MENU_OPTION_FIFTH, STRING_SETTINGS_MENU_OPTION_SIXTH, STRING_SETTINGS_MENU_OPTION_SEVENTH});
MenuHelper ReadyModule::gameModeMenuHelper(STRING_GAME_MENU_TITLE, {STRING_GAME_MENU_OPTION_FIRST, STRING_GAME_MENU_OPTION_SECOND, STRING_GAME_MENU_OPTION_THIRD, STRING_GAME_MENU_OPTION_FOURTH, STRING_GAME_MENU_OPTION_FIFTH});

char ReadyModule::menuIndex = 0;

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
    // Reset our menu pages to zero;
    mainMenuHelper.setIndex(0);
    settingsMenuHelper.setIndex(0);

    menuIndex = 0;

    Serial.println("I've woked!");

    mainMenuHelper.drawMenu();

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
    bool triggerButtonPressed = !InputManager::getInputState(InputSource::BUTTON_TRIGGER);

    // Decide upon left/right navigation
    if (leftButtonPressed && (time - lastButtonPress) > 250)
    {
        // Button is pressed - track time, increment value
        lastButtonPress = time;
        switch (menuIndex)
        {
        case 0:
            mainMenuHelper.moveUp();
            break;
        case 1:
            settingsMenuHelper.moveUp();
            break;
        case 2:
            gameModeMenuHelper.moveUp();
            break;
        default:
            break;
        }
        hasChanged = true;
    }
    else if (rightButtonPressed && (time - lastButtonPress) > 250)
    {
        // Button is pressed - track time, increment value
        lastButtonPress = time;
        switch (menuIndex)
        {
        case 0:
            mainMenuHelper.moveDown();
            break;
        case 1:
            settingsMenuHelper.moveDown();
            break;
        case 2:
            gameModeMenuHelper.moveDown();
            break;
        default:
            break;
        }
        hasChanged = true;
    }

    if (triggerButtonPressed && (time - lastButtonPress) > 250)
    {
        // Trigger is pressed. Perform menu operations based upon what was pressed.
        handleMenuSelection();
        hasChanged = true;
    }

    // Update display
    if (hasChanged)
        switch (menuIndex)
        {
        case 0:
            mainMenuHelper.drawMenu();
            break;
        case 1:
            settingsMenuHelper.drawMenu();
            break;
        case 2:
            gameModeMenuHelper.drawMenu();
            break;
        default:
            break;
        }
}

void ReadyModule::handleMenuSelection()
{
    // Handle the logic for whatever selection has been made
    switch (menuIndex)
    {
    case 0:
        // Main Menu options logic
        switch (mainMenuHelper.getMenuIndex())
        {
        case 0:
            // Select Game Mode (move into game mode menu)
            menuIndex = 2;
            break;
        case 1:
            // Pair Targets (move into pairing state)
            StateManager::setSystemState(SystemState::Pair);
            break;
        case 2:
            // Change Settings (move into settings menu)
            menuIndex = 1;
            break;
        case 3:
            // Move into about page
            // TODO: Implement About Page
            break;
        default:
            // Do nothing, really.
            break;
        }
    case 1:
        // Settings Menu options logic
        switch (settingsMenuHelper.getMenuIndex())
        {
        case 0:
            // Mute Audio
            AudioManager::setVolume(0.0);
            break;
        case 1:
            // Audio: 25%
            AudioManager::setVolume(0.25);
            AudioManager::playAudio("/audio/shoot.wav");
            break;
        case 2:
            // Audio 50%
            AudioManager::setVolume(0.50);
            AudioManager::playAudio("/audio/shoot.wav");
            break;
        case 3:
            // Audio 75%
            AudioManager::setVolume(0.75);
            AudioManager::playAudio("/audio/shoot.wav");
            break;
        case 4:
            // Audio 100%
            AudioManager::setVolume(1.0);
            AudioManager::playAudio("/audio/shoot.wav");
            break;
        case 5:
            // Reset all configuration settings
            break;
        case 6:
            // Back
            menuIndex = 0;
            break;
        default:
            break;
        }
        break;
    case 2:
        // Game Mode options logic
        switch (gameModeMenuHelper.getMenuIndex())
        {
        case 0:
            // One-Shot
            break;
        case 1:
            // Wack-A-mole
            break;
        case 2:
            // Horde
            break;
        case 3:
            // Time Trial
            break;
        case 4:
            // Back
            menuIndex = 0;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}