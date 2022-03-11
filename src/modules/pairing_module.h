#include "module.h"

#include "../managers/lighting_manager.h"
#include "../managers/config_manager.h"

class PairingModule : public Module
{
private:
    static Task moduleTask;

    static void handleNetworkMessage(NetworkMessage message);
    static void handlePhotoInput(InputSource _, bool state);
    static void handlePairingInput(InputSource _, bool state);
public:
    static void initialize(Scheduler *userScheduler);
    static bool onWake();
    static void onSleep();
    static void onUpdate();
    static Task* getTask() { return &moduleTask; }
};