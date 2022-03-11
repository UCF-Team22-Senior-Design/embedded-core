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

    //InputManager::registerInputCallback(&triggerCallback, InputSource::BUTTON_TRIGGER);
    InputManager::registerInputCallback(&leftMenuCallback, InputSource::BUTTON_LEFT);

    // Update display
    DisplayManager::drawBasicScreen("BACK", "", "Pairing Mode", "Trigger to fire");
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

    //InputManager::deregisterInputCallback(&triggerCallback, InputSource::BUTTON_TRIGGER);
    InputManager::deregisterInputCallback(&leftMenuCallback, InputSource::BUTTON_LEFT);
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
    if(state) return;

    // Perform our desired action
    StateManager::setSystemState(SystemState::Ready);
}

void PairingModule::onUpdate()
{
}

void PairingModule::leftMenuCallback(InputSource _, bool state)
{
    // Go back to ready state
    if(!state) return;
    StateManager::setSystemState(SystemState::Ready);
}