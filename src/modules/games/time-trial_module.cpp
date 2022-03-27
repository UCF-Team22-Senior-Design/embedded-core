#include "time-trial_module.h"

Task TimeTrialModule::moduleTask;

int TimeTrialModule::score = 0;
int TimeTrialModule::numTargetsIgnited = 0;
unsigned long TimeTrialModule::startTime = 0;

void TimeTrialModule::initialize(Scheduler *userScheduler)
{
    moduleTask.set(TASK_SECOND, TASK_FOREVER, &onUpdate, &onWake, &onSleep);

    // Bind it to our scheduler
    (*userScheduler).addTask(moduleTask);
}

bool TimeTrialModule::onWake()
{
    Serial.println("[TimeTrialModule] I've been woken from sleep");

    // Reset our variables
    score = 0;
    numTargetsIgnited = ConfigManager::configData.targets.size();
    startTime = millis();

    // Enable laser
    LaserManager::setLaserEnable(true);

    // Register our callbacks
    InputManager::registerInputCallback(&handleLeftMenuInput, InputSource::BUTTON_LEFT);
    NetworkManager::registerCallback(&handleNetworkMessage);

    // Start the game for all targets
    NetworkMessage message(MESSAGE_TAG_GAME_START, "", NetworkManager::getNodeTime());
    NetworkManager::sendMessage(message);

    // Ignite all targets
    String lightingEffect = LightingEncoder::encodeLightingEffect(0, 0, 0, NetworkManager::getNodeTime(), 0, 0, LightingEncoder::rgbToUint(0, 255, 0), LightingEncoder::rgbToUint(0, 0, 0));
    message = NetworkMessage(MESSAGE_TAG_TARGET_IGNITE, lightingEffect, NetworkManager::getNodeTime());
    NetworkManager::sendMessage(message);

    // Give all targets a "flash" lighting effect
    //lightingEffect = LightingEncoder::encodeLightingEffect(1, 0, 0, NetworkManager::getNodeTime(), 0, 100, LightingEncoder::rgbToUint(255, 255, 255), LightingEncoder::rgbToUint(10, 10, 10));
    message = NetworkMessage(MESSAGE_TAG_TARGET_ON_HIT, "-", NetworkManager::getNodeTime());
    NetworkManager::sendMessage(message);

    drawScreen();

    return true;
}

void TimeTrialModule::onSleep()
{
    Serial.println("[TimeTrialModule] I'm being sent to sleep");

    InputManager::deregisterInputCallback(&handleLeftMenuInput, InputSource::BUTTON_LEFT);
    NetworkManager::deregisterCallback(&handleNetworkMessage, "NONE");

    // Disable laser
    LaserManager::setLaserEnable(false);

    // Tell the target to go to sleep
    NetworkMessage gameEndMessage(MESSAGE_TAG_GAME_END, "", NetworkManager::getNodeTime());
    NetworkManager::sendMessage(gameEndMessage);
}


void TimeTrialModule::onUpdate()
{
    if(TIME_TRIAL_LIMIT < (millis() - startTime)) {
        ResultsModule::setResults("Time-Trial", score, "targets");
        StateManager::setSystemState(SystemState::Results);
        return;
    }
    
    drawScreen();      
}

void TimeTrialModule::handleLeftMenuInput(InputSource _, bool state)
{
    // Only act when the input goes from high to low
    if(state) return;

    Serial.println("[TimeTrialModule] Left Menu Pressed!");

    ResultsModule::setResults("Time-Trial", score, "targets");
    Serial.printf("[TimeTrialModule] Lost due to quit, score of %d\n", score);
    StateManager::setSystemState(SystemState::Results);
    return;
}

void TimeTrialModule::handleNetworkMessage(NetworkMessage message)
{
    // Filter to our targets only
    if(ConfigManager::configData.targets.count(message.getSender()) == 0) return;

    // Print the message to serial
    Serial.printf("[TimeTrialModule] Got message: %s", message.toString().c_str());

    // Check if it was a hit!
    if(message.getTag() == MESSAGE_TAG_TARGET_HIT)
    {
        // Decrement num targets lit
        numTargetsIgnited--;

        // If some targets are still alive..
        if(numTargetsIgnited > 0) {
            
            // Extinguish hit target
            String lightingEffect = LightingEncoder::encodeLightingEffect(0, 0, 0, NetworkManager::getNodeTime(), 0, 0, LightingEncoder::rgbToUint(0, 0, 0), LightingEncoder::rgbToUint(0, 0, 0));
            message = NetworkMessage(MESSAGE_TAG_TARGET_EXTINGUISH, lightingEffect, NetworkManager::getNodeTime());
            NetworkManager::sendMessage(message, message.getSender());

        } else {
            
            // Ignite all targets
            String lightingEffect = LightingEncoder::encodeLightingEffect(0, 0, 0, NetworkManager::getNodeTime(), 0, 0, LightingEncoder::rgbToUint(0, 255, 0), LightingEncoder::rgbToUint(0, 0, 0));
            message = NetworkMessage(MESSAGE_TAG_TARGET_IGNITE, lightingEffect, NetworkManager::getNodeTime());
            NetworkManager::sendMessage(message);
            
            // Reset num targets counter
            numTargetsIgnited = ConfigManager::configData.targets.size();
        }

        // Increment score
        score++;

        drawScreen();
    }
}

void TimeTrialModule::drawScreen()
{
    unsigned long timeLeftSecs = (TIME_TRIAL_LIMIT - (millis() - startTime)) / 1000;
    unsigned long timeLeftMins = timeLeftSecs / 60;
    timeLeftSecs = timeLeftSecs % 60;

    String timeMessage = "Time Left: " + String(timeLeftMins) + "m " + (timeLeftSecs < 10 ? "0" : "") + String(timeLeftSecs) + "s \n";
    String scoreMessage = "Score: " + String(score) + " Targets";
    String message = timeMessage + scoreMessage;

    DisplayManager::drawBasicScreen("QUIT", "", "Time-Trial", message.c_str());
}