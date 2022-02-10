#pragma once

#include <painlessMesh.h>

#include "../managers/managers.h"

class Module 
{
protected:
    static Task moduleTask;

    static bool onWake() {return true;};
    static void onSleep() {};
    static void onUpdate() {};
public:
    static void initialize(Scheduler *userScheduler);
    static Task* getTask();
};