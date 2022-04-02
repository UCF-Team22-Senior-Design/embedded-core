#include "pairing_module.h"

Task PairingModule::moduleTask;

/**
 * @brief Register the PairingModule's module task
 * 
 * @param userScheduler 
 */
void PairingModule::initialize(Scheduler *userScheduler)
{
    moduleTask.set(TASK_MILLISECOND * 16, TASK_FOREVER, &onUpdate, &onWake, &onSleep);
    (*userScheduler).addTask(moduleTask);
}

/**
 * @brief Wake up the pairing module from sleep, letting it register callbacks
 * 
 * @return true 
 * @return false 
 */
bool PairingModule::onWake()
{
    Serial.println("[PairingModule] I've been awoken");

    // Enable the laser
    LaserManager::setLaserEnable(true);

    // Register our input callbacks
    //InputManager::registerInputCallback(&triggerCallback, InputSource::BUTTON_TRIGGER);
    InputManager::registerInputCallback(&leftMenuCallback, InputSource::BUTTON_LEFT);
    InputManager::registerInputCallback(&rightMenuCallback, InputSource::BUTTON_RIGHT);
    NetworkManager::registerCallback(networkMessageCallback);

    // Update display
    drawScreen("Shoot a target in \n'Pairing Mode' to \npair it to this \ncontroller");
    return true;
}

/**
 * @brief Signal the pairing module to return to sleep, releasing callbacks
 * 
 */
void PairingModule::onSleep()
{
    // Disable the laser
    LaserManager::setLaserEnable(false);

    // Disable our pairing callbacks
    //InputManager::deregisterInputCallback(&triggerCallback, InputSource::BUTTON_TRIGGER);
    InputManager::deregisterInputCallback(&leftMenuCallback, InputSource::BUTTON_LEFT);
    InputManager::deregisterInputCallback(&rightMenuCallback, InputSource::BUTTON_RIGHT);
    NetworkManager::deregisterCallback(networkMessageCallback, "NONE");

    // Send the "end pairing" message to all clients
    NetworkMessage endPairingMessage("PAIR_COMPLETE", "", NetworkManager::getNodeTime());
    NetworkManager::sendMessage(endPairingMessage);

    Serial.println("[PairingModule] I'm being put to sleep");
}

/**
 * @brief The callback used for trigger events
 * 
 * @param source 
 * @param state 
 */
void PairingModule::triggerCallback(InputSource source, bool state)
{
    // Filter out to only the pull-back of the trigger
    if (state)
        return;

    // Perform our desired action
    StateManager::setSystemState(SystemState::Ready);
}

void PairingModule::onUpdate()
{
    
}

void PairingModule::leftMenuCallback(InputSource _, bool state)
{
    // Go back to ready state
    if (state)
        return;
    StateManager::setSystemState(SystemState::Ready);
}

void PairingModule::rightMenuCallback(InputSource _, bool state)
{
    // Only act on the down-press of the buttons
    if (state)
        return;

    // No point in doing this work if there are no targets to clear.
    if (ConfigManager::configData.targets.size() <= 0)
        return;

    // Unpair all targets
    ConfigManager::configData.targets.clear();
    // Save this change
    ConfigManager::saveData();
    // Refresh display
    drawScreen("Shoot a target in \n'Pairing Mode' to \npair it to this \ncontroller");
}

void PairingModule::networkMessageCallback(NetworkMessage networkMessage)
{
    // Network message things!

    // If it's a target's pairing request, validate it.
    if (networkMessage.getTag() == "PAIR_REQUEST")
    {
        // There's no reason to reject a request right now. If the logic is necessary later, we can do it,
        // but not yet.

        // For now, we register the target and send a confirmation
        ConfigManager::configData.targets.insert(networkMessage.getSender());
        NetworkMessage response("PAIR_ACCEPTED", String(NetworkManager::getNodeID()), NetworkManager::getNodeTime());
        NetworkManager::sendMessage(response, networkMessage.getSender());

        // Store this new target information
        ConfigManager::saveData();
    }
    else if (networkMessage.getTag() == "PAIR_FORGET")
    {
        // If we're being asked to forget this target, do so.
        ConfigManager::configData.targets.erase(networkMessage.getSender());

        // Store our changed information
        ConfigManager::saveData();

        // Update our display
    }

    // For now, just redraw the screen with some info about the message
    String displayMessage = "Sender: " + String(networkMessage.getSender()) + "\nTag: " + networkMessage.getTag() + "\nData: " + networkMessage.getData();
    drawScreen(displayMessage);

    return;
}

void PairingModule::drawScreen(String bodyMessage)
{
    // Build our title message
    String titleMessage = String(ConfigManager::configData.targets.size()) + " targets paired";
    // Draw our message to the screen
    DisplayManager::drawBasicScreen("BACK", "UNPAIR ALL", titleMessage.c_str(), bodyMessage.c_str());
}