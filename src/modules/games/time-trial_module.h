#include "../module.h"

#include "../../managers/display_manager.h"
#include "../../managers/network_manager.h"
#include "../../models/lighting_command.h"
#include "../results_module.h"

#define TIME_TRIAL_LIMIT        60000

class TimeTrialModule : public Module
{
private:
    static Task moduleTask;

    static int score;
    static int numTargetsIgnited;
    static unsigned long startTime;

    static LightingCommand standbyCommand;
    static LightingCommand onHitCommand;

    static void handleNetworkMessage(NetworkMessage message);
    static void handleLeftMenuInput(InputSource _, bool state);

    static void drawScreen();
public:
    static void initialize(Scheduler *userScheduler);
    static bool onWake();
    static void onSleep();
    static void onUpdate();
    static Task* getTask() { return &moduleTask; }
};
