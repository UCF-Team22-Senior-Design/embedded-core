#pragma once

#include <Arduino.h>

#define PWM_FREQUENCY 20000
#define PWM_CHANNEL 0
#define PWM_RESOLUTION 8
#define PWM_PIN_3V3 
#define PWM_PIN_5V
#define PWM_PIN_12V

class PWMManager
{
public:
    static void initialize();
}