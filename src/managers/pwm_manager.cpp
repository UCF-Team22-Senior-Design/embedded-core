#include "pwm_manager.h"

/**
 * @brief Register a channel for the PWM signal, then attach it to all pins.
 *        Honestly, this is equivalent to just setting the pins to 3V3, or, logic high.
 *        But this lets us be a bit more flexible than just setting it to a digital high.
 */
void PWMManager::initialize()
{
    /*
    ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(PWM_PIN_3V3, PWM_CHANNEL);
    */

    pinMode(PWM_PIN_3V3, OUTPUT);
    digitalWrite(PWM_PIN_3V3, 1);
}