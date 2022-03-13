#include "module.h"

#include "../managers/display_manager.h"

class TargetTimingModule : public Module
{
private:
    static Task moduleTask;
    static uint32_t lastFired;
    static uint32_t lastHit;
    static uint32_t lastID;

    static void handleNetworkMessage(NetworkMessage message);
    static void handleTriggerInput(InputSource _, bool state);
    static void handleLeftMenuInput(InputSource _, bool state);
    static void handleRightMenuInput(InputSource _, bool state);
    
    static void refreshDisplay();
public:
    static void initialize(Scheduler *userScheduler);
    static bool onWake();
    static void onSleep();
    static void onUpdate();
    static Task* getTask() { return &moduleTask; }
};
