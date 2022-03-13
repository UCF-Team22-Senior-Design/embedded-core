#include "module.h"

#include "../managers/lighting_manager.h"

class PlayModule : public Module
{
private:
    static Task moduleTask;

    static void handleNetworkMessage(NetworkMessage message);
    static void handlePhotoInput(InputSource _, bool state);
public:
    static void initialize(Scheduler *userScheduler);
    static bool onWake();
    static void onSleep();
    static void onUpdate();
    static Task* getTask() { return &moduleTask; }
};