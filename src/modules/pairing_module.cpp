#include "pairing_module.h"

Task PairingModule::moduleTask;

void PairingModule::initialize(Scheduler *userScheduler)
{
    moduleTask.set(TASK_MILLISECOND * 16, TASK_FOREVER, &onUpdate, &onWake, &onSleep);
    (*userScheduler).addTask(moduleTask);
}

bool PairingModule::onWake()
{
    Serial.println("[PairingModule] I've been awoken");

    InputManager::registerInputCallback(&triggerCallback, InputSource::BUTTON_TRIGGER);

    // Update display
    DisplayManager::drawBasicScreen("", "", "Pairing Mode", "Return to menu");
    return true;
}

void PairingModule::onSleep()
{
    InputManager::deregisterInputCallback(&triggerCallback, InputSource::BUTTON_TRIGGER);
    Serial.println("[PairingModule] I'm being put to sleep");
}

void PairingModule::triggerCallback(InputSource source, bool state)
{
    Serial.printf("[PairingModule] Trigger pull - state %d. System state %d\n", state, StateManager::getSystemState());
    if(!state)
        StateManager::setSystemState(SystemState::Ready);
}

void PairingModule::onUpdate()
{
    static unsigned long lastDrawn = 0;
    if(millis() - lastDrawn > 1000)
    {
        DisplayManager::drawBasicScreen("", "", "Pairing Mode", "Return to menu");
        lastDrawn = millis();
    }
}