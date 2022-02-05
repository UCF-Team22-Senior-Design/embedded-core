#include "lighting_manager.h"

LightingPattern LightingManager::currentLightingPattern;
bool LightingManager::loop;
uint32_t LightingManager::timeStart;
uint32_t LightingManager::timeOut;



Task LightingManager::taskLightingUpdate(TASK_MILLISECOND * 100, TASK_FOREVER, &lightingUpdate);

void LightingManager::initialize(Scheduler *scheduler)
{
    // Initialize the LEDs
    // Set our default values for loop / timeStart/End/lightingPattern
    loop = false;
    timeStart = 0;
    timeOut = 0;
    currentLightingPattern = LightingPattern::OFF;

    // Assign our task to the taskScheduler
    scheduler->addTask(taskLightingUpdate);
}

void LightingManager::stopPattern()
{
    // Disable the task
    taskLightingUpdate.disable();
}

void LightingManager::startPattern()
{
    // Track when playback begins
    timeStart = millis();
    
    // Restart the task
    taskLightingUpdate.restart();
}

void LightingManager::setColor(int r, int g, int b)
{

}