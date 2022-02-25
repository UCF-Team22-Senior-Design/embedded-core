#include "module.h"

#include "../managers/display_manager.h"

class ReadyModule : public Module
{
private:
    static int currentMenuPage;
public:
    static void initialize(Scheduler *userScheduler);
    static bool onWake();
    static void onSleep();
    static void onUpdate();
};