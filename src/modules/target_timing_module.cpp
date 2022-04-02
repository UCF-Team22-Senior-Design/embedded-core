#include "target_timing_module.h"

Task TargetTimingModule::moduleTask;

LightingCommand TargetTimingModule::standbyCommand(LightingPattern::STATIC, 0, 0, NetworkManager::getNodeTime(), 0, 0, LightingCommand::rgbToUint(0, 255, 0), LightingCommand::rgbToUint(0, 0, 0));
LightingCommand TargetTimingModule::onHitCommand(LightingPattern::BLINK_ALL, 0, 0, NetworkManager::getNodeTime(), 200, 100, LightingCommand::rgbToUint(255, 255, 255), LightingCommand::rgbToUint(0, 0, 0));

uint32_t TargetTimingModule::lastFired = 0;
uint32_t TargetTimingModule::lastHit = 0;
uint32_t TargetTimingModule::lastID = 0;

void TargetTimingModule::initialize(Scheduler *userScheduler)
{
    moduleTask.set(TASK_MILLISECOND * 16, TASK_FOREVER, &onUpdate, &onWake, &onSleep);

    // Bind it to our scheduler
    (*userScheduler).addTask(moduleTask);
}

bool TargetTimingModule::onWake()
{
    Serial.println("[TargetTimingModule] I've been woken from sleep");

    // Enable laser
    LaserManager::setLaserEnable(true);

    // Zero-out our data
    lastFired = 0;
    lastHit = 0;
    lastID = 0;

    // Register our callbacks
    InputManager::registerInputCallback(&handleTriggerInput, InputSource::BUTTON_TRIGGER);
    InputManager::registerInputCallback(&handleLeftMenuInput, InputSource::BUTTON_LEFT);
    InputManager::registerInputCallback(&handleRightMenuInput, InputSource::BUTTON_RIGHT);
    NetworkManager::registerCallback(&handleNetworkMessage);

    // Start the game for all targets
    NetworkMessage message(MESSAGE_TAG_GAME_START, "", NetworkManager::getNodeTime());
    NetworkManager::sendMessage(message);

    // Ignite all targets
    message = NetworkMessage(MESSAGE_TAG_TARGET_IGNITE, standbyCommand.toString(), NetworkManager::getNodeTime());
    NetworkManager::sendMessage(message);

    // Give all targets a "flash" lighting effect
    message = NetworkMessage(MESSAGE_TAG_TARGET_ON_HIT, onHitCommand.toString(), NetworkManager::getNodeTime());
    NetworkManager::sendMessage(message);

    // Refresh display!
    refreshDisplay();

    return true;
}

void TargetTimingModule::onSleep()
{
    Serial.println("[TargetTimingModule] I'm being sent to sleep");

    // Disable laser
    LaserManager::setLaserEnable(false);

    // Deregister callbacks
    InputManager::deregisterInputCallback(&handleTriggerInput, InputSource::BUTTON_TRIGGER);
    InputManager::deregisterInputCallback(&handleLeftMenuInput, InputSource::BUTTON_LEFT);
    InputManager::deregisterInputCallback(&handleRightMenuInput, InputSource::BUTTON_RIGHT);
    NetworkManager::deregisterCallback(&handleNetworkMessage, "NONE");

    NetworkMessage gameEndMessage(MESSAGE_TAG_GAME_END, "", NetworkManager::getNodeTime());
    NetworkManager::sendMessage(gameEndMessage);
}

void TargetTimingModule::onUpdate()
{
    // Nothing to do
}

void TargetTimingModule::handleTriggerInput(InputSource _, bool state)
{
    // Only act when the input goes from high to low
    if (state)
        return;
        
    // Register the fire time
    lastFired = NetworkManager::getNodeTime();
}

void TargetTimingModule::handleLeftMenuInput(InputSource _, bool state)
{
    // Only act when the input goes from high to low
    if (state)
        return;

    // Exit the state
    StateManager::setSystemState(SystemState::Ready);
}

void TargetTimingModule::handleRightMenuInput(InputSource _, bool state)
{
    // Only act when the input goes from high to low
    if (state)
        return;

    // Only act when there's data to clear
    if(lastFired >= lastHit) return;

    lastFired = 0;
    lastHit = 0;
    lastID = 0;

    refreshDisplay();
}

void TargetTimingModule::handleNetworkMessage(NetworkMessage message)
{
    // Filter to our targets only
    if(ConfigManager::configData.targets.count(message.getSender()) == 0) return;

    // Print the message to serial
    Serial.printf("[TargetTimingModule] Got message: %s\n", message.toString().c_str());

    // Check if it was a target hit message
    if(message.getTag() == MESSAGE_TAG_TARGET_HIT)
    {
        // A target has been hit! Record the hit time, hit target
        lastID = message.getSender();
        lastHit = message.getTimestamp();
        // Refresh the display
        refreshDisplay();
    }
}

void TargetTimingModule::refreshDisplay()
{
    // Update the display with the last-known target hit
    if(lastFired >= lastHit)
    {
        // We haven't fired the gun yet - or, we've fired the gun and not hit
        // anything yet.
        DisplayManager::drawBasicScreen("BACK", "", "Shot Timer", "Shoot a target to \nrecord data");
        return;
    }

    // Calculate the time between the last shot and last hit.
    uint32_t deltaTime = lastHit - lastFired;
    String bodyString = "TargetID: " + String(lastID) + "\nTrigger: " + String(lastFired) + "ns\nHit: " + String(lastHit) + "ns\nDelta: " + String(deltaTime) + "ns";
    DisplayManager::drawBasicScreen("BACK", "CLEAR", "Shot Timer", bodyString.c_str());
}