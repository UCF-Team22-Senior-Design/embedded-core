#include "module.h"

#include "../managers/display_manager.h"
#include "../managers/network_manager.h"
#include "../models/lighting_command.h"

class StandbyTestModule : public Module
{
private:
    static Task moduleTask;

    static LightingCommand standbyCommand;
    static LightingCommand onHitCommand;

    static unsigned long startTime;

    static void handleNetworkMessage(NetworkMessage message);
    static void handleRightMenuInput(InputSource _, bool state);

    static void refreshDisplay();
public:
    static void initialize(Scheduler *userScheduler);
    static bool onWake();
    static void onSleep();
    static void onUpdate();
    static Task* getTask() { return &moduleTask; }
};
