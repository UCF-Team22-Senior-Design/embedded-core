#include "ready_module.h"

Task ReadyModule::moduleTask;

const char *ReadyModule::STRING_MENU_TITLE = "Main Menu";
const char *ReadyModule::STRING_MENU_OPTION_FIRST = "Select Game Mode";
const char *ReadyModule::STRING_MENU_OPTION_SECOND = "Pair Targets";
const char *ReadyModule::STRING_MENU_OPTION_THIRD = "Change Settings";
const char *ReadyModule::STRING_MENU_OPTION_FOURTH = "About";
const char *ReadyModule::STRING_MENU_OPTION_FIFTH = "Target Timing";

const char *ReadyModule::STRING_SETTINGS_MENU_TITLE = "Settings";
const char *ReadyModule::STRING_SETTINGS_MENU_OPTION_FIRST = "Mute Audio";
const char *ReadyModule::STRING_SETTINGS_MENU_OPTION_SECOND = "12.5% Volume";
const char *ReadyModule::STRING_SETTINGS_MENU_OPTION_THIRD = "25% Volume";
const char *ReadyModule::STRING_SETTINGS_MENU_OPTION_FOURTH = "50% Volume";
const char *ReadyModule::STRING_SETTINGS_MENU_OPTION_FIFTH = "100% Volume";
const char *ReadyModule::STRING_SETTINGS_MENU_OPTION_SIXTH = "Laser On";
const char *ReadyModule::STRING_SETTINGS_MENU_OPTION_SEVENTH = "Laser Off";
const char *ReadyModule::STRING_SETTINGS_MENU_OPTION_EIGHTH = "Laser Pulse";
const char *ReadyModule::STRING_SETTINGS_MENU_OPTION_NINTH = "Laser Follow";
const char *ReadyModule::STRING_SETTINGS_MENU_OPTION_TENTH = "Reset ALL Settings";
const char *ReadyModule::STRING_SETTINGS_MENU_OPTION_ELEVENTH = "Go Back";

const char *ReadyModule::STRING_GAME_MENU_TITLE = "Select Game Mode";
const char *ReadyModule::STRING_GAME_MENU_OPTION_FIRST = "One-Shot";
const char *ReadyModule::STRING_GAME_MENU_OPTION_SECOND = "Whack-A-Mole";
const char *ReadyModule::STRING_GAME_MENU_OPTION_THIRD = "Horde";
const char *ReadyModule::STRING_GAME_MENU_OPTION_FOURTH = "Time Trial";
const char *ReadyModule::STRING_GAME_MENU_OPTION_FIFTH = "Back";

MenuHelper ReadyModule::mainMenuHelper(STRING_MENU_TITLE, {STRING_MENU_OPTION_FIRST, STRING_MENU_OPTION_SECOND, STRING_MENU_OPTION_THIRD, STRING_MENU_OPTION_FOURTH, STRING_MENU_OPTION_FIFTH});
MenuHelper ReadyModule::settingsMenuHelper(STRING_SETTINGS_MENU_TITLE, {STRING_SETTINGS_MENU_OPTION_FIRST, STRING_SETTINGS_MENU_OPTION_SECOND, STRING_SETTINGS_MENU_OPTION_THIRD, STRING_SETTINGS_MENU_OPTION_FOURTH, STRING_SETTINGS_MENU_OPTION_FIFTH, STRING_SETTINGS_MENU_OPTION_SIXTH, STRING_SETTINGS_MENU_OPTION_SEVENTH, STRING_SETTINGS_MENU_OPTION_EIGHTH, STRING_SETTINGS_MENU_OPTION_NINTH, STRING_SETTINGS_MENU_OPTION_TENTH, STRING_SETTINGS_MENU_OPTION_ELEVENTH});
MenuHelper ReadyModule::gameModeMenuHelper(STRING_GAME_MENU_TITLE, {STRING_GAME_MENU_OPTION_FIRST, STRING_GAME_MENU_OPTION_SECOND, STRING_GAME_MENU_OPTION_THIRD, STRING_GAME_MENU_OPTION_FOURTH, STRING_GAME_MENU_OPTION_FIFTH});

ReadyModuleMenuState ReadyModule::menuIndex = ReadyModuleMenuState::MAIN;

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

    // Set our main menu index (what master menu we are in) to its initial state
    menuIndex = ReadyModuleMenuState::MAIN;

    // Register our input callbacks
    InputManager::registerInputCallback(handleTriggerPull, InputSource::BUTTON_TRIGGER);
    InputManager::registerInputCallback(handleMenuLeft, InputSource::BUTTON_LEFT);
    InputManager::registerInputCallback(handleMenuRight, InputSource::BUTTON_RIGHT);

    Serial.println("[ReadyModule] I've been asked to wake up");

    // Draw the initial menu frame
    mainMenuHelper.drawMenu();

    return true;
}

/**
 * @brief This function is called when the readyModule task is disabled. Use it
 *        to store changes, reset variables, etc.
 */
void ReadyModule::onSleep()
{
    // Deregister our input callbacks
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
    // I've got nothing, suprisingly
}

/**
 * @brief The callback used to handle incoming trigger pull events.
 * 
 * @param _ InputSource - filtered to always be trigger
 * @param state The new, changed state of the source
 */
void ReadyModule::handleTriggerPull(InputSource _, bool state)
{
    // The trigger's state has fallen from high to low
    if (state)
        return;

    if (handleMenuSelection())
        refreshDisplay();
}

bool ReadyModule::handleMenuSelection()
{
    Serial.printf("[ReadyModule] Trigger pulled, menu index %d and sub-menu index ", menuIndex);
    // Handle the logic for whatever selection has been made
    switch (menuIndex)
    {
    case MAIN:
        // Main Menu options logic
        Serial.printf("%d\n", mainMenuHelper.getMenuIndex());
        return handleMainMenuSelection();
        break;
    case SETTINGS:
        // Settings Menu options logic
        Serial.printf("%d\n", settingsMenuHelper.getMenuIndex());
        return handleSettingMenuSelection();
        break;
    case GAME:
        // Game Mode options logic
        Serial.printf("%d\n", gameModeMenuHelper.getMenuIndex());
        return handleGameMenuSelection();
        break;
    default:
        break;
    }

    return true;
}

bool ReadyModule::handleMainMenuSelection()
{
    switch (mainMenuHelper.getMenuIndex())
    {
    case 0:
        // Select Game Mode (move into game mode menu)
        menuIndex = ReadyModuleMenuState::GAME;
        return true;
        break;
    case 1:
        // Pair Targets (move into pairing state)
        StateManager::setSystemState(SystemState::Pair);
        break;
    case 2:
        // Change Settings (move into settings menu)
        menuIndex = ReadyModuleMenuState::SETTINGS;
        return true;
        break;
    case 3:
        // Move into about page
        // TODO: Implement About Page
        break;
    case 4:
        // Move into target timing page
        StateManager::setSystemState(SystemState::TargetTiming);
        break;
    default:
        // Do nothing, really.
        break;
    }

    return false;
}

bool ReadyModule::handleSettingMenuSelection()
{
    switch (settingsMenuHelper.getMenuIndex())
    {
    case 0:
        // Mute Audio
        AudioManager::setVolume(0.0);
        break;
    case 1:
        // Audio: 12.5%
        AudioManager::setVolume(0.125);
        AudioManager::playAudio("/audio/shoot.wav");
        break;
    case 2:
        // Audio 25%
        AudioManager::setVolume(0.25);
        AudioManager::playAudio("/audio/shoot.wav");
        break;
    case 3:
        // Audio 50%
        AudioManager::setVolume(0.50);
        AudioManager::playAudio("/audio/shoot.wav");
        break;
    case 4:
        // Audio 100%
        AudioManager::setVolume(1.0);
        AudioManager::playAudio("/audio/shoot.wav");
        break;
    case 5:
        // Laser On
        LaserManager::setLaserMode(LaserMode::ON);
        break;
    case 6:
        // Laser Off
        LaserManager::setLaserMode(LaserMode::OFF);
        break;
    case 8:
        // laser Follow Trigger
        LaserManager::setLaserMode(LaserMode::FOLLOWS_TRIGGER);
        break;
    case 7:
        // Laser Pulse
        LaserManager::setLaserMode(LaserMode::PULSE_ON_TRIGGER);
        break;
    case 9:
        // Reset all configuration settings
        ConfigManager::resetData();
        break;
    case 10:
        // Back
        menuIndex = ReadyModuleMenuState::MAIN;
        return true;
        break;
    default:
        break;
    }
    return false;
}

bool ReadyModule::handleGameMenuSelection()
{
    switch (gameModeMenuHelper.getMenuIndex())
    {
    case 0:
        // One-Shot
        StateManager::setSystemState(SystemState::GameOneShot);
        break;
    case 1:
        // Wack-A-mole
        StateManager::setSystemState(SystemState::GameWhackAMole);
        break;
    case 2:
        // Horde
        StateManager::setSystemState(SystemState::GameHorde);
        break;
    case 3:
        // Time Trial
        StateManager::setSystemState(SystemState::GameTimeTrial);
        break;
    case 4:
        // Back
        menuIndex = ReadyModuleMenuState::MAIN;
        return true;
        break;
    default:
        break;
    }

    return false;
}

void ReadyModule::refreshDisplay()
{
    switch (menuIndex)
    {
    case MAIN:
        mainMenuHelper.drawMenu();
        break;
    case SETTINGS:
        settingsMenuHelper.drawMenu();
        break;
    case GAME:
        gameModeMenuHelper.drawMenu();
        break;
    default:
        break;
    }
}

void ReadyModule::handleMenuRight(InputSource _, bool state)
{
    // Only care when there's a change to high
    if (state)
        return;

    // If so, do our navigation logic
    switch (menuIndex)
    {
    case MAIN:
        mainMenuHelper.moveUp();
        break;
    case SETTINGS:
        settingsMenuHelper.moveUp();
        break;
    case GAME:
        gameModeMenuHelper.moveUp();
        break;
    default:
        break;
    }

    // And refresh the display
    refreshDisplay();
}

void ReadyModule::handleMenuLeft(InputSource _, bool state)
{
    // Only act when changing from low->high
    if (state)
        return;

    switch (menuIndex)
    {
    case MAIN:
        mainMenuHelper.moveDown();
        break;
    case SETTINGS:
        settingsMenuHelper.moveDown();
        break;
    case GAME:
        gameModeMenuHelper.moveDown();
        break;
    default:
        break;
    }

    // Refresh the display, no matter
    refreshDisplay();
}