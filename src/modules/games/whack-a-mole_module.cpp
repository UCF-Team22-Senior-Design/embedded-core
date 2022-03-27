#include "whack-a-mole_module.h"

Task WhackAMoleModule::moduleTask;

int WhackAMoleModule::score = 0;
unsigned long WhackAMoleModule::startTime = 0;

void WhackAMoleModule::initialize(Scheduler *userScheduler)
{
    moduleTask.set(TASK_SECOND, TASK_FOREVER, &onUpdate, &onWake, &onSleep);

    // Bind it to our scheduler
    (*userScheduler).addTask(moduleTask);
}

bool WhackAMoleModule::onWake()
{
    Serial.println("[WhackAMoleModule] I've been woken from sleep");

    // Reset our variables
    score = 0;
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
    lightingEffect = LightingEncoder::encodeLightingEffect(1, 0, 0, NetworkManager::getNodeTime(), 0, 100, LightingEncoder::rgbToUint(255, 255, 255), LightingEncoder::rgbToUint(10, 10, 10));
    message = NetworkMessage(MESSAGE_TAG_TARGET_ON_HIT, lightingEffect, NetworkManager::getNodeTime());
    NetworkManager::sendMessage(message);

    drawScreen();

    return true;
}

void WhackAMoleModule::onSleep()
{
    Serial.println("[WhackAMoleModule] I'm being sent to sleep");

    InputManager::deregisterInputCallback(&handleLeftMenuInput, InputSource::BUTTON_LEFT);
    NetworkManager::deregisterCallback(&handleNetworkMessage, "NONE");

    // Disable laser
    LaserManager::setLaserEnable(false);

    // Tell the target to go to sleep
    NetworkMessage gameEndMessage(MESSAGE_TAG_GAME_END, "", NetworkManager::getNodeTime());
    NetworkManager::sendMessage(gameEndMessage);
}


void WhackAMoleModule::onUpdate()
{
    // End game once time limit is exceeded
    if(WHACK_A_MOLE_LIMIT < (millis() - startTime)) {
        ResultsModule::setResults("Whack-A-Mole", score, "targets");
        StateManager::setSystemState(SystemState::Results);
        return;
    }
     
    // Extinguish all targets (they have been active too long)
    String lightingEffect = LightingEncoder::encodeLightingEffect(0, 0, 0, NetworkManager::getNodeTime(), 0, 0, LightingEncoder::rgbToUint(0, 0, 0), LightingEncoder::rgbToUint(0, 0, 0));
    NetworkMessage message = NetworkMessage(MESSAGE_TAG_TARGET_EXTINGUISH, lightingEffect, NetworkManager::getNodeTime());
    NetworkManager::sendMessage(message);
    
    // Randomly ignite 1 - 4 targets
    lightingEffect = LightingEncoder::encodeLightingEffect(1, 0, 0, NetworkManager::getNodeTime() + 500000, 0, 0, LightingEncoder::rgbToUint(0, 255, 0), LightingEncoder::rgbToUint(0, 0, 0));
    message = NetworkMessage(MESSAGE_TAG_TARGET_IGNITE, lightingEffect, NetworkManager::getNodeTime());
    // 25% chance of igniting any individual target, at least 1 target must be lit
    bool noTargetsIgnited = true;
    while(noTargetsIgnited) {
        for(auto target : ConfigManager::configData.targets) {
            if(rand() % 4 == 3) {
                NetworkManager::sendMessage(message, target);
                noTargetsIgnited = false;
            }
        }
    }

    drawScreen(); 
}

void WhackAMoleModule::handleLeftMenuInput(InputSource _, bool state)
{
    // Only act when the input goes from high to low
    if(state) return;

    Serial.println("[WhackAMoleModule] Left Menu Pressed!");

    ResultsModule::setResults("Whack-A-Mole", score, "targets");
    Serial.printf("[WhackAMoleModule] Lost due to quit, score of %d\n", score);
    StateManager::setSystemState(SystemState::Results);
    return;
}

void WhackAMoleModule::handleNetworkMessage(NetworkMessage message)
{
    // Filter to our targets only
    if(ConfigManager::configData.targets.count(message.getSender()) == 0) return;

    // Print the message to serial
    Serial.printf("[WhackAMoleModule] Got message: %s\n", message.toString().c_str());

    // Check if it was a hit!
    if(message.getTag() == MESSAGE_TAG_TARGET_HIT)
    {
        // Extinguish hit target
        String lightingEffect = LightingEncoder::encodeLightingEffect(0, 0, 0, NetworkManager::getNodeTime(), 0, 0, LightingEncoder::rgbToUint(0, 0, 0), LightingEncoder::rgbToUint(0, 0, 0));
        message = NetworkMessage(MESSAGE_TAG_TARGET_EXTINGUISH, lightingEffect, NetworkManager::getNodeTime());
        NetworkManager::sendMessage(message, message.getSender());

        // Increment score
        score++;

        // Update Play screen
        drawScreen();
    }
}

void WhackAMoleModule::drawScreen()
{
    unsigned long timeLeftSecs = (WHACK_A_MOLE_LIMIT - (millis() - startTime)) / 1000;
    unsigned long timeLeftMins = timeLeftSecs / 60;
    timeLeftSecs = timeLeftSecs % 60;

    String timeMessage = "Time Left: " + String(timeLeftMins) + "m " + (timeLeftSecs < 10 ? "0" : "") + String(timeLeftSecs) + "s \n";
    String scoreMessage = "Score: " + String(score) + " Targets";
    String message = timeMessage + scoreMessage;

    DisplayManager::drawBasicScreen("QUIT", "", "Whack-A-Mole", message.c_str());
}