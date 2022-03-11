#include "ready_module.h"

Task ReadyModule::moduleTask;

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

    Serial.println("[ReadyModule] I've been woken from sleep");

    LightingManager::setLoop(false);
    LightingManager::setClearOnStop(false);

    // Set the lights to be a static green color or a static red color, depending on pairing state
    bool isPaired = (ConfigManager::configData.controller != 0);
    if (isPaired)
    {
        LightingManager::setPrimaryColor(0, 120, 0);
    }
    else
    {
        LightingManager::setPrimaryColor(120, 0, 0);
    }

    LightingManager::setSecondaryColor(0, 0, 0);
    LightingManager::setPattern(LightingPattern::STATIC);
    LightingManager::startPattern();

    // Register our callbacks
    InputManager::registerInputCallback(&handlePhotoInput, InputSource::PHOTOTRANSISTOR);
    InputManager::registerInputCallback(&handlePairingInput, InputSource::BUTTON_PAIR);
    NetworkManager::registerCallback(&handleNetworkMessage);

    return true;
}

/**
 * @brief This function is called when the readyModule task is disabled. Use it
 *        to store changes, reset variables, etc.
 */
void ReadyModule::onSleep()
{
    // Put to sleep whatever we have been running
    Serial.println("[ReadyModule] I'm being sent to sleep");

    // De-register callbacks
    InputManager::deregisterInputCallback(&handlePhotoInput, InputSource::PHOTOTRANSISTOR);
    InputManager::deregisterInputCallback(&handlePairingInput, InputSource::BUTTON_PAIR);
    NetworkManager::deregisterCallback(&handleNetworkMessage, "NONE");
}

/**
 * @brief Runs every "frame" - perform fixed update things, such as checking
 *        inputs, updating screen, etc.
 */
void ReadyModule::onUpdate()
{
    // Nothing to doooooo
}

void ReadyModule::handlePhotoInput(InputSource _, bool state)
{
    // Only act when the input goes from low to high
    if (state)
        return;

    Serial.println("[ReadyModule] I've been shot!");

    // Currently? blink white once then return to our normal color
    LightingManager::stopPattern();
    LightingManager::setPattern(LightingPattern::BLINK_ALL);
    LightingManager::setPrimaryColor(255, 255, 255);
    
    bool isPaired = (ConfigManager::configData.controller != 0);
    if (isPaired)
    {
        LightingManager::setSecondaryColor(0, 120, 0);
    }
    else
    {
        LightingManager::setSecondaryColor(120, 0, 0);
    }
    
    LightingManager::startPattern();
}

void ReadyModule::handlePairingInput(InputSource _, bool state)
{
    // Only act when the input goes from high to low
    if (!state)
        return;

    // Shift into the pairing state
    StateManager::setSystemState(SystemState::Pair);
}

void ReadyModule::handleNetworkMessage(NetworkMessage message)
{
    // Just print it out - in the futue, jump into the game mode when we recieve a gamemode message
    Serial.printf("[ReadyModule] Got message: %s\n", message.toString().c_str());
}