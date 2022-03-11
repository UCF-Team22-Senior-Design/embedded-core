#include "pairing_module.h"

Task PairingModule::moduleTask;

void PairingModule::initialize(Scheduler *userScheduler)
{
    moduleTask.set(TASK_MILLISECOND * 16, TASK_FOREVER, &onUpdate, &onWake, &onSleep);

    // Bind it to our scheduler
    (*userScheduler).addTask(moduleTask);
}

bool PairingModule::onWake()
{
    Serial.println("[PairingModule] I've been woken from sleep");

    // Clear our existing pairing
    ConfigManager::configData.controller = 0;
    ConfigManager::saveData();

    // Set the lights to be a static yellow color
    LightingManager::setLoop(false);
    LightingManager::setClearOnStop(false);
    LightingManager::setPrimaryColor(120, 120, 0);
    LightingManager::setSecondaryColor(0, 0, 0);
    LightingManager::setPattern(LightingPattern::STATIC);
    LightingManager::startPattern();

    // Register our callbacks
    InputManager::registerInputCallback(&handlePhotoInput, InputSource::PHOTOTRANSISTOR);
    InputManager::registerInputCallback(&handlePairingInput, InputSource::BUTTON_PAIR);
    NetworkManager::registerCallback(&handleNetworkMessage);

    return true;
}

void PairingModule::onSleep()
{
    Serial.println("[PairingModule] I'm being sent to sleep");

    InputManager::deregisterInputCallback(&handlePhotoInput, InputSource::PHOTOTRANSISTOR);
    InputManager::deregisterInputCallback(&handlePairingInput, InputSource::BUTTON_PAIR);
    NetworkManager::deregisterCallback(&handleNetworkMessage, "NONE");
}


void PairingModule::onUpdate()
{
    // Nothing to do
}

void PairingModule::handlePhotoInput(InputSource _, bool state)
{
    // Only act when the input goes from low to high
    if(state) return;

    Serial.println("[PairingModule] I've been shot!");

    // Currently? blink white once then return to our normal color
    LightingManager::stopPattern();
    LightingManager::setPattern(LightingPattern::BLINK_ALL);
    LightingManager::setPrimaryColor(255, 255, 255);
    LightingManager::setSecondaryColor(120, 120, 0);
    LightingManager::startPattern();

    // Also release a pairing request packet if we're not currently paired
    if(ConfigManager::configData.controller == 0)
    {
        NetworkMessage hitMessage("PAIR_REQUEST", "", NetworkManager::getNodeTime());
        NetworkManager::sendMessage(hitMessage);
    }
}

void PairingModule::handlePairingInput(InputSource _, bool state)
{
    // Only act when the input goes from high to low
    if(!state) return;

    // Save our current settings
    ConfigManager::saveData();

    // Shift into the ready state
    StateManager::setSystemState(SystemState::Ready);
}

void PairingModule::handleNetworkMessage(NetworkMessage message)
{
    // Print the message to serial
    Serial.printf("[PairingModule] Got message: %s\n", message.toString().c_str());

    // Parse it
    if(message.getTag() == "PAIR_ACCEPT")
    {
        // We've been accepted by the controller. Store its ID, and shift back
        // into ready state
        ConfigManager::configData.controller = message.getSender();
        
        ConfigManager::saveData();
        return;
    }

    if(message.getTag() == "PAIR_COMPLETE")
    {
        // Pairing is complete. Return to the ready state
        StateManager::setSystemState(SystemState::Ready);
        return;
    }
}
