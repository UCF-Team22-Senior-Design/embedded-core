#include "ready_module.h"

Task ReadyModule::moduleTask;

/**
 * @brief Establish our task, bind it to the scheduler.
 * 
 * @param userScheduler 
 */
void ReadyModule::initialize(Scheduler *userScheduler)
{
    moduleTask.set(TASK_MILLISECOND * 16, TASK_FOREVER, &onUpdate, &onWake, &onSleep);

    // Bind it to our scheduler
    (*userScheduler).addTask(moduleTask);
}

/**
 * @brief This function is called when the readyModule task is enabled. Use it to
 *        prepare variables (load from configuration data), reset operating things,
 *        and whatever else you'd like. Must return true, otherwise the task will
 *        not wake up and the system will be dead.
 */
bool ReadyModule::onWake()
{

    Serial.println("I've woked!");
    
  LightingManager::setLoop(true);
  LightingManager::setPrimaryColor(120, 0, 120);
  LightingManager::setSecondaryColor(0, 0, 0);
  LightingManager::setPattern(LightingPattern::MARCHING_BLINK);
  LightingManager::startPattern();

    return true;
}

/**
 * @brief This function is called when the readyModule task is disabled. Use it
 *        to store changes, reset variables, etc.
 */
void ReadyModule::onSleep()
{
    // Put to sleep whatever we have been running
    Serial.println("I sleep");
}

/**
 * @brief Runs every "frame" - perform fixed update things, such as checking 
 *        inputs, updating screen, etc.
 */
void ReadyModule::onUpdate()
{
    static unsigned long lastMillis = millis();
    unsigned long time = millis();
    unsigned long deltaTime = time - lastMillis;
    lastMillis = time;

    //Serial.printf("ReadyModule Update: <ΔT %lums>\n", deltaTime);

    static unsigned long lastShift = 0;
    static char patternIndex = 0;
    if(time - lastShift > 5000)
    {
        lastShift = time;
        // Change our lighting pattern
        if(patternIndex > 2) patternIndex = 0;
        //Serial.printf("Lighting Pattern %d\n", patternIndex);
        //LightingManager::setPattern(static_cast<LightingPattern>(patternIndex++));
    }
}