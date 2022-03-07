#include "module.h"

#include "../managers/display_manager.h"

class PairingModule : public Module
{
private:
    // Nothing for now - No secrets <3
    static Task moduleTask;

    static void triggerCallback(InputSource source, bool state);
public:
    static void initialize(Scheduler *userScheduler);
    static bool onWake();
    static void onSleep();
    static void onUpdate();
    static Task* getTask() {return &moduleTask;};
};