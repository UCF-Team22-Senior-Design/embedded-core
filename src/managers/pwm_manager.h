#pragma once

#include <Arduino.h>

#define PWM_FREQUENCY 20000
#define PWM_CHANNEL 0
#define PWM_RESOLUTION 8
#define PWM_CYCLE 255

#ifdef CONTROLLER
    #define PWM_PIN_3V3 33
#else
    #define PWM_PIN_3V3 12
    #define PWM_PIN_5V  33
    #define PWM_PIN_12V 27
#endif

class PWMManager
{
public:
    static void initialize();
};