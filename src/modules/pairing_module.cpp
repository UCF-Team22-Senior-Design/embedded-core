#include "pairing_module.h"

Task PairingModule::moduleTask;

void PairingModule::initialize(Scheduler *userScheduler)
{
    moduleTask.set(TASK_SECOND, TASK_FOREVER, &onUpdate, &onWake, &onSleep);

    // Bind it to our scheduler
    (*userScheduler).addTask(moduleTask);
}

bool PairingModule::onWake()
{
    Serial.println("[PairingModule] I've been woken from sleep");

    // Clear our existing pairing
    ConfigManager::configData.controller = 0;
    ConfigManager::saveData();

    // Issue a general forget request
    NetworkMessage forgetMessage("PAIR_FORGET", String(NetworkManager::getNodeID()), NetworkManager::getNodeTime());
    NetworkManager::sendMessage(forgetMessage);

    // Set the lights to be a static yellow color
    LightingManager::setStandbyLightingCommand(LightingCommand(
        LightingPattern::STATIC, false, false, 0, 0, 0, LightingCommand::rgbToUint(255, 255, 0), LightingCommand::rgbToUint(0, 0, 0)));
    // Establish our blinky command
    LightingManager::setOnHitLightingCommand(LightingCommand(
        LightingPattern::BLINK_ALL, false, false, 0, 201, 100, LightingCommand::rgbToUint(255, 255, 255), LightingCommand::rgbToUint(120, 120, 0)));
    
    // Start our standby
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
    if(message.getTag() == "PAIR_ACCEPTED")
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

    if(message.getTag() == "PAIR_FORGET" && message.getSender() == ConfigManager::configData.controller)
    {
        // Remove the controller's pairing information
        ConfigManager::configData.controller = 0;
        ConfigManager::saveData();
    }
}
