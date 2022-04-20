#include "horde_module.h"

Task HordeModule::moduleTask;

LightingCommand HordeModule::standbyCommand(LightingPattern::STATIC, 0, 0, NetworkManager::getNodeTime(), 0, 100, LightingCommand::rgbToUint(0, 0, 0), LightingCommand::rgbToUint(0, 0, 0));
LightingCommand HordeModule::onHitCommand(LightingPattern::BLINK_ALL, 0, 0, NetworkManager::getNodeTime(), 200, 100, LightingCommand::rgbToUint(255, 255, 255), LightingCommand::rgbToUint(0, 0, 0));

/*

Horde mode:
 - Targets light up one at a time
 - As the target "lives," decrement its state (by changing its light color)
 - If the target decreases by three states, you lose.
 - Shooting a target causes it to die, saving you

*/

int HordeModule::score = 0;
int HordeModule::zombieSpeed = 0;
float HordeModule::spawnFactor = 1.0;
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
    zombieSpeed = TARGET_INCREMENT_VALUE;

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

    // Extinguish all targets
    standbyCommand.primaryColor = 0;
    message = NetworkMessage(MESSAGE_TAG_TARGET_EXTINGUISH, standbyCommand.toString(), NetworkManager::getNodeTime());
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
    standbyCommand.primaryColor = 0;
    NetworkMessage message(MESSAGE_TAG_TARGET_EXTINGUISH, standbyCommand.toString(), NetworkManager::getNodeTime());
    NetworkManager::sendMessage(message);

    NetworkMessage gameEndMessage(MESSAGE_TAG_GAME_END, "", NetworkManager::getNodeTime());
    NetworkManager::sendMessage(gameEndMessage);
}


void HordeModule::onUpdate()
{
    // Check if we need to spawn a zombie
    if(timeOfNextSpawn != 0 && timeOfNextSpawn < millis())
    {

        Serial.printf("We need to spawn a zombie\n");

        // Spawn a new zombie from our list of inactive ones
        std::vector<uint32_t> possibleZombies;
        for(auto zombieID : ConfigManager::configData.targets)
        {
            if(targetStates[static_cast<unsigned long>(zombieID)] == 0)
                possibleZombies.push_back(zombieID);
        }

        if(!possibleZombies.empty())
        {
            Serial.printf("We can spawn a zombie\n");
            // Pick one to spawn
            int spawnIndex = rand() % possibleZombies.size();

            uint32_t selectedZombie = possibleZombies[spawnIndex];
            unsigned long ulongSelectedZombie = static_cast<unsigned long>(selectedZombie);

            Serial.printf("We're spawning a zombie with ID %lu\n", selectedZombie);

            // Spawn it, by
            // ... setting its state to 1
            targetStates[ulongSelectedZombie] = 1;
            // ... tracking the time of spawn
            targetActivationTime[ulongSelectedZombie] = millis();
            // ... and sending the activation message

            standbyCommand.pattern = LightingPattern::STATIC;
            standbyCommand.primaryColor = LightingCommand::rgbToUint(0, 255, 0);

            NetworkMessage activationMessage = NetworkMessage(MESSAGE_TAG_TARGET_IGNITE, standbyCommand.toString(), NetworkManager::getNodeTime());
            NetworkManager::sendMessage(activationMessage, selectedZombie);
        }

        // Reset the spawn timer to anywhere from one to six seconds away
        timeOfNextSpawn = millis() + (rand() % 5000 + 500) * spawnFactor;
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
                    color = LightingCommand::rgbToUint(255, 255, 0);
                    break;
                case 3:
                case 4:
                    // Turn red
                    color = LightingCommand::rgbToUint(255, 0, 0);
                    break;
                default:
                    color = LightingCommand::rgbToUint(0, 255, 0);
                    break;
            }

            standbyCommand.primaryColor = color;
            standbyCommand.secondaryColor = LightingCommand::rgbToUint(255, 255, 255);

            String lightingEffect;
            if(targetStates[longTarget] == 4)
            {
                standbyCommand.pattern = LightingPattern::BLINK_ALL;
            } else
            {
                standbyCommand.pattern = LightingPattern::STATIC;
            }

            NetworkMessage igniteMessage(MESSAGE_TAG_TARGET_IGNITE, standbyCommand.toString(), NetworkManager::getNodeTime());
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

    static unsigned long lastRefresh = 0;
    if(millis() - lastRefresh > 1000)
    {
        // Refresh the display
        drawScreen();
        lastRefresh = millis();
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
        standbyCommand.primaryColor = 0;
        NetworkMessage extinguishMessage(MESSAGE_TAG_TARGET_EXTINGUISH, standbyCommand.toString(), NetworkManager::getNodeTime());
        NetworkManager::sendMessage(extinguishMessage, message.getSender());

        // If the target was hit, extinguish it, add it to our score
        // Reset the target's state to zero
        targetStates[static_cast<unsigned long>(message.getSender())] = 0;
        score++;

        // Play a successful hit event noise
        AudioManager::playAudio("/audio/hit-success.wav");

        // Speed up the zombies
        zombieSpeed = zombieSpeed * 0.90;
        spawnFactor = spawnFactor * 0.90;
        // Clamp the zombie speed
        if(spawnFactor < 0.1) spawnFactor = 0.1;
        if(zombieSpeed < 200) zombieSpeed = 200;

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