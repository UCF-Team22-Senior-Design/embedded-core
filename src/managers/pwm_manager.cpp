#include "pwm_manager.h"

/**
 * @brief Register a channel for the PWM signal, then attach it to all pins.
 *        Honestly, this is equivalent to just setting the pins to 3V3, or, logic high.
 *        But this lets us be a bit more flexible than just setting it to a digital high.
 */
void PWMManager::initialize()
{
    //ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    pinMode(PWM_PIN_5V, OUTPUT);
    pinMode(PWM_PIN_12V, OUTPUT);
    digitalWrite(PWM_PIN_5V, true);
    digitalWrite(PWM_PIN_12V, true);
}