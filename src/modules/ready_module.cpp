#include "ready_module.h"

int ReadyModule::currentMenuPage = 0;

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
    // Reset our menu page to zero;
    currentMenuPage = 0;

    Serial.println("I've woked!");

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

    Serial.printf("Ready Module Update [∆T: %lums]", deltaTime);

    // Do some dummy thing - listen to a button, I guess, and increment the menu
    // if the button is pressed. If menu reaches a value, reset the task.
    static unsigned long lastButtonPress = 0;
    bool buttonPressed = !InputManager::getInputState(InputSource::EXAMPLE);
    if(buttonPressed && (time - lastButtonPress) > 250)
    {
        // Button is pressed - track time, increment value
        lastButtonPress = time;
        currentMenuPage++;
    }

    // Print out current menu page
    Serial.printf(" currentMenuPage: %d\n", currentMenuPage);

    if(currentMenuPage > 10)
    {
        StateManager::setSystemState(SystemState::Ready);
    }
}