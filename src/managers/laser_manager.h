#pragma once

#include <Arduino.h>
#include "input_manager.h"

// Controller laser is driven by three pins.
#define PIN_LASER1  12
#define PIN_LASER2  14
#define PIN_LASER3  27

class LaserManager
{
private:
    static bool hasBeenInitialized;
    static void triggerCallback(InputSource source, bool state);
public:
    static void initialize();
};