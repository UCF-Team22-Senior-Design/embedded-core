#include "standby_tester_module.h"

Task StandbyTestModule::moduleTask;

unsigned long StandbyTestModule::startTime;

LightingCommand StandbyTestModule::standbyCommand(LightingPattern::STATIC, 0, 0, NetworkManager::getNodeTime(), 0, 0, LightingCommand::rgbToUint(0, 255, 0), LightingCommand::rgbToUint(0, 0, 0));
LightingCommand StandbyTestModule::onHitCommand(LightingPattern::BLINK_ALL, 0, 0, NetworkManager::getNodeTime(), 200, 100, LightingCommand::rgbToUint(255, 255, 255), LightingCommand::rgbToUint(0, 0, 0));


void StandbyTestModule::initialize(Scheduler *userScheduler)
{
    moduleTask.set(TASK_SECOND * 0.5, TASK_FOREVER, &onUpdate, &onWake, &onSleep);

    // Bind it to our scheduler
    (*userScheduler).addTask(moduleTask);
}

bool StandbyTestModule::onWake()
{
    Serial.println("[StandbyTestModule] I've been woken from sleep");

    // Enable laser
    LaserManager::setLaserEnable(true);

    startTime = millis();

    // Register our callbacks
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

void StandbyTestModule::onSleep()
{
    Serial.println("[StandbyTestModule] I'm being sent to sleep");

    // Disable laser
    LaserManager::setLaserEnable(false);

    // Set targets to sleep
    NetworkMessage gameEndMessage(MESSAGE_TAG_GAME_END, "", NetworkManager::getNodeTime());
    NetworkManager::sendMessage(gameEndMessage);

    InputManager::deregisterInputCallback(&handleRightMenuInput, InputSource::BUTTON_RIGHT);
    NetworkManager::deregisterCallback(&handleNetworkMessage, "NONE");
}


void StandbyTestModule::onUpdate()
{
    static bool triggerState = false;
    // Toggle trigger state
    triggerState = !triggerState;

    // Set trigger state
    InputManager::simulateButtonPress(InputSource::BUTTON_TRIGGER, triggerState);

    // Refresh display
    refreshDisplay();
}

void StandbyTestModule::handleRightMenuInput(InputSource _, bool state)
{
    // Only act when the input goes from high to low
    if(state) return;

    Serial.println("[StandbyTestModule] Right Menu Pressed!");

    // Leave
    StateManager::setSystemState(SystemState::Ready);
}

void StandbyTestModule::handleNetworkMessage(NetworkMessage message)
{

    // Print the message to serial
    Serial.printf("[StandbyTestModule] Got message: %s\n", message.toString().c_str());
}

void StandbyTestModule::refreshDisplay()
{
    
    unsigned long timeLiveSecs = (millis() - startTime) / 1000;
    unsigned long timeLiveMins = timeLiveSecs / 60;
    unsigned long timeLiveHours = timeLiveMins / 60;
    timeLiveSecs = timeLiveSecs % 60;

    String timeMessage = String("Uptime: ") + (timeLiveHours < 10 ? "0" : "") + String(timeLiveHours) + "h " + String(timeLiveMins) + "m " + (timeLiveSecs < 10 ? "0" : "") + String(timeLiveSecs) + "s \n";
    String scoreMessage = "State: " + String(InputManager::getInputState(InputSource::BUTTON_TRIGGER));
    String message = timeMessage + scoreMessage;

    DisplayManager::drawBasicScreen("QUIT", "", "Standby Test", message.c_str());

}