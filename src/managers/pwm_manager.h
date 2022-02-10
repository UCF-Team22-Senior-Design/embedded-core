#pragma once

#include <Arduino.h>

#include "../models/pinouts.h"

#define PWM_FREQUENCY 20000
#define PWM_CHANNEL 0
#define PWM_RESOLUTION 8
#define PWM_CYCLE 255

class PWMManager
{
public:
    static void initialize();
};