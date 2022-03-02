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
    currentMenuPage = 1;

    Serial.println("I've woked!");
    //DisplayManager::drawBasicScreen("PAIR", "", "Too Few Targets", "Pair at least 2 targets");
    DisplayManager::drawFourOptionSelectScreen("Select Game Mode", "Time Trial", "One Shot", "Option3", "Option4", 1, "PAIR", "CONFIRM");

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

    /*Serial.printf("Ready Module Update [∆T: %lums] ", deltaTime);
    Serial.printf("%d%d%d\n", InputManager::getInputState(InputSource::BUTTON_LEFT), 
        InputManager::getInputState(InputSource::BUTTON_TRIGGER), 
        InputManager::getInputState(InputSource::BUTTON_RIGHT));
*/
    static unsigned long lastButtonPressLeft = 0;
    static unsigned long lastButtonPressRight = 0;

    int lastMenuPage = currentMenuPage;
    bool leftButtonPressed = !InputManager::getInputState(InputSource::BUTTON_LEFT);
    if(leftButtonPressed && (time - lastButtonPressLeft) > 250)
    {
        // Button is pressed - track time, increment value
        lastButtonPressLeft = time;
        currentMenuPage--;
    }

    bool rightButtonPressed = !InputManager::getInputState(InputSource::BUTTON_RIGHT);
    if(rightButtonPressed && (time - lastButtonPressRight) > 250)
    {
        // Button is pressed - track time, increment value
        lastButtonPressRight = time;
        currentMenuPage++;
    }

    // Print out current menu page/
    //Serial.printf(" currentMenuPage: %d\n", currentMenuPage);
    if(currentMenuPage > 4)
    {
        currentMenuPage = 1;
    }
    else if (currentMenuPage < 1) 
    {
        currentMenuPage = 4;
    }
    
    if(lastMenuPage != currentMenuPage)
        //DisplayManager::drawSimpleScreen("Page " + String(currentMenuPage), 1);
        DisplayManager::drawFourOptionSelectScreen("Select Game Mode", "Time Trial", "One Shot", "Option3", "Option4", currentMenuPage, "PAIR", "CONFIRM");
}