#include "module.h"

#include "../managers/display_manager.h"
#include "../managers/network_manager.h"

class AboutModule : public Module
{
private:
    static Task moduleTask;

    static int modulePage;
    static const int numPages = 3;

    static void handleLeftMenuInput(InputSource _, bool state);
    static void handleRightMenuInput(InputSource _, bool state);

    static void refreshDisplay();
public:
    static void initialize(Scheduler *userScheduler);
    static bool onWake();
    static void onSleep();
    static void onUpdate();
    static Task* getTask() { return &moduleTask; }
};