#include <unordered_map>

#include "../module.h"

#include "../../managers/display_manager.h"
#include "../../managers/network_manager.h"
#include "../../models/lighting_command.h"
#include "../results_module.h"


#define TARGET_INCREMENT_VALUE 8000

class HordeModule : public Module
{
private:
    static Task moduleTask;

    static int score;
    /* Target State Possibilities:
     * 0 - dead / inactive
     * 1 - active / far
     * 2 - active / mid
     * 3 - active / close
     * 4 - active / critical
     * 5 - over
     */
    static std::unordered_map<unsigned long, char> targetStates;
    static std::unordered_map<unsigned long, unsigned long> targetActivationTime;
    static unsigned long timeOfNextSpawn;
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
