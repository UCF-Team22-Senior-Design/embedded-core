#pragma once

#include <Arduino.h>
#include "input_manager.h"

#include "../models/pinouts.h"

class LaserManager
{
private:
    static bool hasBeenInitialized;
    static void triggerCallback(InputSource source, bool state);
public:
    static void initialize();
};