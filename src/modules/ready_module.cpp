#include "ready_module.h"

Task ReadyModule::moduleTask;

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

    InputManager::registerInputCallback(handleTriggerPull, InputSource::BUTTON_TRIGGER);
    InputManager::registerInputCallback(handleMenuLeft, InputSource::BUTTON_LEFT);
    InputManager::registerInputCallback(handleMenuRight, InputSource::BUTTON_RIGHT);

    Serial.println("[ReadyModule] I've been asked to wake up");

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
    InputManager::deregisterInputCallback(handleTriggerPull, InputSource::BUTTON_TRIGGER);
    InputManager::deregisterInputCallback(handleMenuLeft, InputSource::BUTTON_LEFT);
    InputManager::deregisterInputCallback(handleMenuRight, InputSource::BUTTON_RIGHT);
    Serial.println("[ReadyModule] Being sent to sleep");
}

/**
 * @brief Runs every "frame" - perform fixed update things, such as checking
 *        inputs, updating screen, etc.
 */
void ReadyModule::onUpdate()
{
    // I've got nothing.
}

bool ReadyModule::handleMenuSelection()
{
    Serial.printf("[ReadyModule] Trigger pulled, menu index %d and sub-menu index ", menuIndex);
    // Handle the logic for whatever selection has been made
    switch (menuIndex)
    {
    case 0:
        // Main Menu options logic
        Serial.printf("%d\n", mainMenuHelper.getMenuIndex());
        switch (mainMenuHelper.getMenuIndex())
        {
        case 0:
            // Select Game Mode (move into game mode menu)
            menuIndex = 2;
            return true;
            break;
        case 1:
            // Pair Targets (move into pairing state)
            StateManager::setSystemState(SystemState::Pair);
            return false;
            break;
        case 2:
            // Change Settings (move into settings menu)
            menuIndex = 1;
            return true;
            break;
        case 3:
            // Move into about page
            // TODO: Implement About Page
            break;
        default:
            // Do nothing, really.
            break;
        }
        break;
    case 1:
        // Settings Menu options logic
        Serial.printf("%d\n", settingsMenuHelper.getMenuIndex());
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
            return true;
            break;
        default:
            break;
        }
        break;
    case 2:
        // Game Mode options logic
        Serial.printf("%d\n", gameModeMenuHelper.getMenuIndex());
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
            return true;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }

    return true;
}

void ReadyModule::refreshDisplay()
{
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

void ReadyModule::handleTriggerPull(InputSource _, bool state)
{
    // The trigger has been pulled. Whoop de doo.
    if(state) return;

    if(handleMenuSelection())
        refreshDisplay();
}

void ReadyModule::handleMenuLeft(InputSource _, bool state)
{
    // Only care when there's a change to high
    if(!state) return;

    // If so, do our navigation logic
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

    // And refresh the display
    refreshDisplay();
}

void ReadyModule::handleMenuRight(InputSource _, bool state)
{
    if(!state) return;

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

    refreshDisplay();
}