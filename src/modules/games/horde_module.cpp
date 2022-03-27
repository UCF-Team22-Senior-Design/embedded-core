#include "horde_module.h"

Task HordeModule::moduleTask;

/*

Horde mode:
 - Targets light up one at a time
 - As the target "lives," decrement its state (by changing its light color)
 - If the target decreases by three states, you lose.
 - Shooting a target causes it to die, saving you

*/

int HordeModule::score = 0;
    /* Target State Possibilities:
     * 0 - dead / inactive
     * 1 - active / far
     * 2 - active / mid
     * 3 - active / close
     * 4 - active / critical
     * 5 - over
     */
std::unordered_map<unsigned long, char> HordeModule::targetStates;
std::unordered_map<unsigned long, unsigned long> HordeModule::targetActivationTime;
unsigned long HordeModule::timeOfNextSpawn = 0;
unsigned long HordeModule::startTime = 0;

void HordeModule::initialize(Scheduler *userScheduler)
{
    moduleTask.set(TASK_MILLISECOND * 16, TASK_FOREVER, &onUpdate, &onWake, &onSleep);

    // Bind it to our scheduler
    (*userScheduler).addTask(moduleTask);
}

bool HordeModule::onWake()
{
    Serial.println("[HordeModule] I've been woken from sleep");

    // Reset our score
    score = 0;
    startTime = millis();

    // Set our timer to spawn a zombie when we start
    timeOfNextSpawn = millis();
    
    // Initialize our target states to zero
    targetStates.clear();
    targetActivationTime.clear();
    for(uint32_t target : ConfigManager::configData.targets)
    {
        targetStates[static_cast<unsigned long>(target)] = 0;
        targetActivationTime[static_cast<unsigned long>(target)] = 0;
    }

    // Register our callbacks
    InputManager::registerInputCallback(&handleLeftMenuInput, InputSource::BUTTON_LEFT);
    NetworkManager::registerCallback(&handleNetworkMessage);

    // Enable laser output
    LaserManager::setLaserEnable(true);

    // Start the game for all targets
    NetworkMessage message(MESSAGE_TAG_GAME_START, "", NetworkManager::getNodeTime());
    NetworkManager::sendMessage(message);

    drawScreen();

    return true;
}

void HordeModule::onSleep()
{
    Serial.println("[HordeModule] I'm being sent to sleep");

    InputManager::deregisterInputCallback(&handleLeftMenuInput, InputSource::BUTTON_LEFT);
    NetworkManager::deregisterCallback(&handleNetworkMessage, "NONE");
    
    // Disable laser output
    LaserManager::setLaserEnable(false);

    // Tell all targets to go to sleep
    NetworkMessage gameEndMessage(MESSAGE_TAG_GAME_END, "", NetworkManager::getNodeTime());
    NetworkManager::sendMessage(gameEndMessage);
}


void HordeModule::onUpdate()
{
    // Check if we need to spawn a zombie
    if(timeOfNextSpawn != 0 && timeOfNextSpawn < millis())
    {
        // Spawn a new zombie from our list of inactive ones
        std::vector<uint32_t> possibleZombies;
        for(auto zombieID : ConfigManager::configData.targets)
        {
            if(targetStates[static_cast<unsigned long>(zombieID)] == 0)
                possibleZombies.push_back(zombieID);
        }

        if(!possibleZombies.empty())
        {
            // Pick one to spawn
            int spawnIndex = rand() % possibleZombies.size();

            uint32_t selectedZombie = possibleZombies[spawnIndex];
            unsigned long ulongSelectedZombie = static_cast<unsigned long>(selectedZombie);

            // Spawn it, by
            // ... setting its state to 1
            targetStates[ulongSelectedZombie] = 1;
            // ... tracking the time of spawn
            targetActivationTime[ulongSelectedZombie] = millis();
            // ... and sending the activation message

            String lightingEffect = LightingEncoder::encodeLightingEffect(0, 0, 0, NetworkManager::getNodeTime(), 0, 0, LightingEncoder::rgbToUint(0, 255, 0), LightingEncoder::rgbToUint(0, 0, 0));
            NetworkMessage activationMessage = NetworkMessage(MESSAGE_TAG_TARGET_IGNITE, lightingEffect, NetworkManager::getNodeTime());
            NetworkManager::sendMessage(activationMessage, selectedZombie);
        }

        // Reset the spawn timer
        timeOfNextSpawn = millis() + rand() % 5000 + 1000;
    }
    
    // Change the state of all zombies who need to move to the next state
    for(auto target : ConfigManager::configData.targets)
    {
        // Only do math if their target state is > 0
        unsigned long longTarget = static_cast<unsigned long>(target);
        if(targetStates[longTarget] <= 0) continue;

        // Check to see if it's been TARGET_INCREMENT_VALUE since their last activation
        if(millis() - targetActivationTime[longTarget] > TARGET_INCREMENT_VALUE)
        {
            // If so, increment their state and update their visual
            targetStates[longTarget]++;
            targetActivationTime[longTarget] = millis();

            Serial.printf("State change: %d\n", targetStates[longTarget]);

            uint32_t color;
            switch(targetStates[longTarget])
            {
                case 2:
                    // Turn yellow
                    color = LightingEncoder::rgbToUint(255, 255, 0);
                    break;
                case 3:
                case 4:
                    // Turn red
                    color = LightingEncoder::rgbToUint(255, 0, 0);
                    break;
                default:
                    color = LightingEncoder::rgbToUint(0, 255, 0);
                    break;
            }

            String lightingEffect;
            if(targetStates[longTarget] == 4)
            {
                lightingEffect = LightingEncoder::encodeLightingEffect(1, 1, 0, NetworkManager::getNodeTime(), 0, 200, color, LightingEncoder::rgbToUint(255, 255, 255));
            } else
            {
                lightingEffect = LightingEncoder::encodeLightingEffect(0, 0, 0, NetworkManager::getNodeTime(), 0, 0, color, LightingEncoder::rgbToUint(255, 255, 255));
            }

            NetworkMessage igniteMessage(MESSAGE_TAG_TARGET_IGNITE, lightingEffect, NetworkManager::getNodeTime());
            NetworkManager::sendMessage(igniteMessage, target);
        }

        if(targetStates[longTarget] == 5)
        {
            // We're dead
            ResultsModule::setResults("Horde", score, "zombies");
            StateManager::setSystemState(SystemState::Results);
            return;
        }
    }
}

void HordeModule::handleLeftMenuInput(InputSource _, bool state)
{
    // Only act when the input goes from high to low
    if(state) return;

    Serial.println("[HordeModule] Left Menu Pressed!");

    // User is quitting the game
    ResultsModule::setResults("Horde", score, "zombies");
    StateManager::setSystemState(SystemState::Results);

    return;
}

void HordeModule::handleNetworkMessage(NetworkMessage message)
{
    // Filter to just our targets
    if(ConfigManager::configData.targets.count(message.getSender()) == 0) return;

    // Print the message to serial
    Serial.printf("[HordeModule] Got message: %s\n", message.toString().c_str());

    if(message.getTag() == MESSAGE_TAG_TARGET_HIT)
    {
        // If the target was hit, extinguish it, add it to our score
        // Reset the target's state to zero
        targetStates[static_cast<unsigned long>(message.getSender())] = 0;
        score++;
        drawScreen();
    }
}

void HordeModule::drawScreen()
{
    
    unsigned long timeLiveSecs = (millis() - startTime) / 1000;
    unsigned long timeLiveMins = timeLiveSecs / 60;
    timeLiveSecs = timeLiveSecs % 60;

    String timeMessage = "Time: " + String(timeLiveMins) + "m " + (timeLiveSecs < 10 ? "0" : "") + String(timeLiveSecs) + "s \n";
    String scoreMessage = "Score: " + String(score) + " Zombies";
    String message = timeMessage + scoreMessage;

    DisplayManager::drawBasicScreen("QUIT", "", "Horde", message.c_str());
}