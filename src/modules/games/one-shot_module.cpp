#include "one-shot_module.h"

Task OneShotModule::moduleTask;

bool OneShotModule::hasShot = false;
int OneShotModule::score = 0;
unsigned long OneShotModule::lastShotTime = 0;

void OneShotModule::initialize(Scheduler *userScheduler)
{
    moduleTask.set(TASK_MILLISECOND * 500, TASK_FOREVER, &onUpdate, &onWake, &onSleep);

    // Bind it to our scheduler
    (*userScheduler).addTask(moduleTask);
}

bool OneShotModule::onWake()
{
    Serial.println("[OneShotModule] I've been woken from sleep");

    // Reset our variables
    hasShot = false;
    score = 0;
    lastShotTime = 0;

    LaserManager::setLaserEnable(true);

    // Register our callbacks
    InputManager::registerInputCallback(&handleTriggerInput, InputSource::BUTTON_TRIGGER);
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

void OneShotModule::onSleep()
{
    Serial.println("[OneShotModule] I'm being sent to sleep");

    InputManager::deregisterInputCallback(&handleTriggerInput, InputSource::BUTTON_TRIGGER);
    InputManager::deregisterInputCallback(&handleLeftMenuInput, InputSource::BUTTON_LEFT);
    NetworkManager::deregisterCallback(&handleNetworkMessage, "NONE");

    LaserManager::setLaserEnable(false);

    // Tell the target to go to sleep
    NetworkMessage gameEndMessage(MESSAGE_TAG_GAME_END, "", NetworkManager::getNodeTime());
    NetworkManager::sendMessage(gameEndMessage);
}


void OneShotModule::onUpdate()
{
    // If "hasShot" is true & time between last shot & now is greater than timeout, end game
    if(hasShot && (millis() - lastShotTime > 500))
    {
        // User has lost - send them to results page.
        ResultsModule::setResults("One-Shot", score, "targets");
        StateManager::setSystemState(SystemState::Results);
        Serial.printf("[OneShotModule] Lost due to timeout after miss, score of %d\n", score);
    }
}

void OneShotModule::handleTriggerInput(InputSource _, bool state)
{
    // Only act when the input goes from high to low
    if(state) return;

    Serial.println("[OneShotModule] Trigger Pulled!");

    // Check if we've shot already
    if(hasShot)
    {
        // The user has missed and is firing again
        // Send them to the shadow realm - er, results page
        Serial.printf("[OneShotModule] Lost due to trigger pull after miss, score of %d\n", score);
        ResultsModule::setResults("One-Shot", score, "targets");
        StateManager::setSystemState(SystemState::Results);
        return;
    }

    hasShot = true;
    lastShotTime = millis();
}

void OneShotModule::handleLeftMenuInput(InputSource _, bool state)
{
    // Only act when the input goes from high to low
    if(state) return;

    Serial.println("[OneShotModule] Left Menu Pressed!");

    ResultsModule::setResults("One-Shot", score, "targets");
    Serial.printf("[OneShotModule] Lost due to quit, score of %d\n", score);
    StateManager::setSystemState(SystemState::Results);
    return;
}

void OneShotModule::handleNetworkMessage(NetworkMessage message)
{
    // Filter to our targets only
    if(ConfigManager::configData.targets.count(message.getSender()) == 0) return;

    // Print the message to serial
    Serial.printf("[OneShotModule] Got message: %s", message.toString().c_str());

    // Check if it was a hit!
    if(message.getTag() == MESSAGE_TAG_TARGET_HIT)
    {
        // Reset our shot
        hasShot = false;
        // Increment score
        score++;

        drawScreen();
    }
}

void OneShotModule::drawScreen()
{
    String scoreMessage = "Score: " + String(score) + " Targets";
    DisplayManager::drawBasicScreen("QUIT", "", "One-Shot", scoreMessage.c_str());
}