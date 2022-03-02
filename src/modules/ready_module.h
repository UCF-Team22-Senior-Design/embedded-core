#include "module.h"

#include "../managers/lighting_manager.h"

class ReadyModule : public Module
{
private:
public:
    static void initialize(Scheduler *userScheduler);
    static bool onWake();
    static void onSleep();
    static void onUpdate();
};