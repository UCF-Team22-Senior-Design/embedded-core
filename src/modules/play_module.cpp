#include "play_module.h"

Task PlayModule::moduleTask;
bool PlayModule::enabled;

void PlayModule::initialize(Scheduler *userScheduler)
{
    moduleTask.set(TASK_MILLISECOND * 16, TASK_FOREVER, &onUpdate, &onWake, &onSleep);

    // Bind it to our scheduler
    (*userScheduler).addTask(moduleTask);
}

bool PlayModule::onWake()
{
    Serial.println("[PlayModule] I've been woken from sleep");

    // Return to defaults
    enabled = false;

    // Register our callbacks
    InputManager::registerInputCallback(&handlePhotoInput, InputSource::PHOTOTRANSISTOR);
    NetworkManager::registerCallback(&handleNetworkMessage);

    return true;
}

void PlayModule::onSleep()
{
    Serial.println("[PlayModule] I'm being sent to sleep");

    InputManager::deregisterInputCallback(&handlePhotoInput, InputSource::PHOTOTRANSISTOR);
    NetworkManager::deregisterCallback(&handleNetworkMessage, "NONE");
}


void PlayModule::onUpdate()
{
    // Nothing to do
}

void PlayModule::handlePhotoInput(InputSource _, bool state)
{
    // Only act when the input goes from low to high
    if(state) return;

    // Only act if we're enabled
    if(!enabled) return;

    // Send our "hit" message to the controller
    NetworkMessage hitMessage("TARGET_HIT", "", NetworkManager::getNodeTime());
    NetworkManager::sendMessage(hitMessage, ConfigManager::configData.controller);

    Serial.println("[PlayModule] I've been shot!");
}

void PlayModule::handleNetworkMessage(NetworkMessage message)
{
    // Filter messages to only our controller
    if(message.getSender() != ConfigManager::configData.controller) return;

    // Print the message to serial
    Serial.printf("[PlayModule] Got message: %s\n", message.toString().c_str());

    if(message.getTag() == "TARGET_ON_HIT")
    {
        LightingCommand onHitCommand;

        // If the message involves a "-", we're setting our onHit to default
        if(message.getData()[0] == '-')
            onHitCommand = LightingCommand(message.getData().c_str());

        LightingManager::setOnHitLightingCommand(onHitCommand);

        return;
    }

    if(message.getTag() == "TARGET_IGNITE" || message.getTag() == "TARGET_EXTINGUISH")
    {
        LightingCommand newEffect(message.getData().c_str());
        // Set the target's lights with the pattern provided
        LightingManager::setStandbyLightingCommand(newEffect);

        // Enable the target if we're ignited, disabled otherwise.
        enabled = (message.getTag() == "TARGET_IGNITE");
        return;
    }

    if(message.getTag() == "GAME_END")
    {
        // End the game, shifting back into the ready module
        StateManager::setSystemState(SystemState::Ready);
    }
}
