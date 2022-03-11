#pragma once

#include <Arduino.h>
#include <painlessMesh.h>

#include "input_manager.h"
#include "audio_manager.h"

#include "../models/pinouts.h"

enum LaserMode
{
    FOLLOWS_TRIGGER,
    PULSE_ON_TRIGGER,
    ON,
    OFF
};

class LaserManager
{
private:
    static void triggerCallback(InputSource source, bool state);
    static void dimLaserCallback();
    static void setLaserState(bool on);

    static bool hasBeenInitialized;
    static LaserMode laserMode;
    static Task taskLaserDimmer;

    static const unsigned long PULSE_LENGTH = 10;
public:
    static void initialize(Scheduler* userScheduler);
    static void setLaserMode(LaserMode newMode);
};