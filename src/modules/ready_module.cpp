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

    LightingCommand lightingCommand(LightingPattern::STATIC, false, false, 0, 201, 0, 0, 0);

    // Set the lights to be a static green color or a static red color, depending on pairing state
    bool isPaired = (ConfigManager::configData.controller != 0);
    if (isPaired)
    {
        lightingCommand.primaryColor = LightingCommand::rgbToUint(0, 120, 0);
    }
    else
    {
        lightingCommand.primaryColor = LightingCommand::rgbToUint(120, 0, 0);
    }

    LightingManager::setStandbyLightingCommand(lightingCommand);

    // Create our blink pattern
    LightingCommand blinkPattern(
        LightingPattern::BLINK_ALL, false, false, 0, 201, 100, LightingCommand::rgbToUint(255, 255, 255), 0
    );

    blinkPattern.secondaryColor = lightingCommand.primaryColor;
    LightingManager::setOnHitLightingCommand(blinkPattern);

    LightingManager::startPattern();

    // Register our callbacks
    InputManager::registerInputCallback(&handlePhotoInput, InputSource::PHOTOTRANSISTOR);
    InputManager::registerInputCallback(&handlePairingInput, InputSource::BUTTON_PAIR);
    NetworkManager::registerCallback(&handleNetworkMessage);

  AudioManager::playAudio("/audio/owin31.wav");

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
}

void ReadyModule::handlePairingInput(InputSource _, bool state)
{
    // Only act when the input goes from high to low
    if (!state)
        return;

    AudioManager::playAudio("/audio/owin31.wav");

    // Shift into the pairing state
    StateManager::setSystemState(SystemState::Pair);
}

void ReadyModule::handleNetworkMessage(NetworkMessage message)
{
    // Filter messages to only our controller
    if(message.getSender() != ConfigManager::configData.controller) return;

    // Just print it out - in the futue, jump into the game mode when we recieve a gamemode message
    Serial.printf("[ReadyModule] Got message: %s\n", message.toString().c_str());

    String messageTag = message.getTag();

    if(messageTag == "GAME_START")
    {
        // Send ourselves into the gameplay module
        StateManager::setSystemState(SystemState::Play);
        return;
    }
}