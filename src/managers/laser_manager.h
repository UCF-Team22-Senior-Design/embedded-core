#pragma once

#include <Arduino.h>
#include "input_manager.h"

// Controller laser is driven by three pins.
#define PIN_LASER1  12
#define PIN_LASER2  13
#define PIN_LASER3  15

class LaserManager
{
private:

    static bool hasBeenInitialized;
    static void triggerCallback(InputSource source, bool state);
public:
    LaserManager();

    static void initialize();
};