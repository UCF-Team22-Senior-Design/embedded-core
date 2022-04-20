#include "about_module.h"

Task AboutModule::moduleTask;
int AboutModule::modulePage = 0;

void AboutModule::initialize(Scheduler *userScheduler)
{
    moduleTask.set(TASK_SECOND, TASK_FOREVER, &onUpdate, &onWake, &onSleep);

    // Bind it to our scheduler
    (*userScheduler).addTask(moduleTask);
}

bool AboutModule::onWake()
{
    Serial.println("[AboutModule] I've been woken from sleep");

    // Register our callbacks
    InputManager::registerInputCallback(&handleLeftMenuInput, InputSource::BUTTON_LEFT);
    InputManager::registerInputCallback(&handleRightMenuInput, InputSource::BUTTON_RIGHT);

    refreshDisplay();

    return true;
}

void AboutModule::onSleep()
{
    Serial.println("[AboutModule] I'm being sent to sleep");

    InputManager::deregisterInputCallback(&handleLeftMenuInput, InputSource::BUTTON_LEFT);
    InputManager::deregisterInputCallback(&handleRightMenuInput, InputSource::BUTTON_RIGHT);
}


void AboutModule::onUpdate()
{
    // Nothing to do
    refreshDisplay();
}

void AboutModule::handleLeftMenuInput(InputSource _, bool state)
{
    // Only act when the input goes from high to low
    if(state) return;

    Serial.println("[AboutModule] Left Menu Pressed!");

    // Exit
    StateManager::setSystemState(SystemState::Ready);
}

void AboutModule::handleRightMenuInput(InputSource _, bool state)
{
    // Only act when the input goes from high to low
    if(state) return;

    Serial.println("[AboutModule] Right Menu Pressed!");

    // Increment page count
    modulePage++;
    if(modulePage >= numPages) modulePage = 0;

    refreshDisplay();
}

void AboutModule::refreshDisplay()
{
    String displayTitle;
    String displayString;
    switch(modulePage)
    {
        case 0:
            displayTitle = "About: Build Info";
            displayString = "WCLSG Project\n2021-2022 UCF SD\nLast Build:\n" + String(__DATE__) + " " + String(__TIME__);
            break;
        case 1:
            displayTitle = "About: Group 22";
            displayString = "Jamauri Balzourt\nRachel Goodman\nAnna Malaj\nThomas Stoeckert";
            break;
        case 2:
        default:
            displayTitle = "About: Status";
            displayString = "Device ID: " + String(NetworkManager::getNodeID()) + "\nPaired: " + String(ConfigManager::configData.targets.size()) + "\nNet: " + String(NetworkManager::getNumNodes()) + "\nNet Time: " + String(NetworkManager::getNodeTime());
            break;
    }

    DisplayManager::drawBasicScreen("BACK", "NEXT", displayTitle.c_str(), displayString.c_str());
}