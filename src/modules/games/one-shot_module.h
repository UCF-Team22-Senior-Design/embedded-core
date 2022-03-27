#include "../module.h"

#include "../../managers/display_manager.h"
#include "../../managers/network_manager.h"
#include "../results_module.h"
#include "../helpers/lighting_encoder.h"

class OneShotModule : public Module
{
private:
    static Task moduleTask;

    static bool hasShot;
    static int score;
    static unsigned long lastShotTime;

    static void handleNetworkMessage(NetworkMessage message);
    static void handleTriggerInput(InputSource _, bool state);
    static void handleLeftMenuInput(InputSource _, bool state);

    static void drawScreen();
public:
    static void initialize(Scheduler *userScheduler);
    static bool onWake();
    static void onSleep();
    static void onUpdate();
    static Task* getTask() { return &moduleTask; }
};
