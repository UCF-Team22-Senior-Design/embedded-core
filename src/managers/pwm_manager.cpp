#include "pwm_manager.h"

/**
 * @brief Register a channel for the PWM signal, then attach it to all pins.
 *        Honestly, this is equivalent to just setting the pins to 3V3, or, logic high.
 *        But this lets us be a bit more flexible than just setting it to a digital high.
 */
void PWMManager::initialize()
{
    ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
#ifdef PLATFORM_CONTROLLER
    ledcAttachPin(PWM_PIN_3V3, PWM_CHANNEL);
#endif
#ifdef PLATFORM_TARGET
    ledcAttachPin(PWM_PIN_5V, PWM_CHANNEL);
    ledcAttachPin(PWM_PIN_12V, PWM_CHANNEL);
#endif
}