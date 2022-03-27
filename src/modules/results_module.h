#pragma once

#include "module.h"

#include "../managers/display_manager.h"
#include "../managers/network_manager.h"

class ResultsModule : public Module
{
private:
    static Task moduleTask;

    static String gamemode;
    static String scoreLabel;
    static int score;


    static void handleLeftMenuInput(InputSource _, bool state);
public:
    static void setResults(String newGamemodeLabel, int newScore, String newScoreLabel);
    static void initialize(Scheduler *userScheduler);
    static bool onWake();
    static void onSleep();
    static void onUpdate();
    static Task* getTask() { return &moduleTask; }
};
