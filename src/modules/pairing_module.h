#include "module.h"

#include "../managers/network_manager.h"
#include "../managers/display_manager.h"

class PairingModule : public Module
{
private:
    static Task moduleTask;

    static void triggerCallback(InputSource source, bool state);
    static void leftMenuCallback(InputSource _, bool state);
    static void networkMessageCallback(NetworkMessage message);

    static void drawScreen(String bodyMessage);
public:
    static void initialize(Scheduler *userScheduler);
    static bool onWake();
    static void onSleep();
    static void onUpdate();
    static Task* getTask() {return &moduleTask;};
};