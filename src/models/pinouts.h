#pragma once

// Shared input mappings
#define PIN_EXAMPLE         15

// Controller input mappings
#define PIN_BUTTON_LEFT     36
#define PIN_BUTTON_RIGHT    39
#define PIN_TRIGGER         22

#define PIN_SDA 21
#define PIN_SCL 22

// Controller output mappings
#define PIN_LASER1  12
#define PIN_LASER2  14
#define PIN_LASER3  27

// Controller PWM mappings
#ifdef PLATFORM_CONTROLLER
    #define PWM_PIN_3V3 33
#endif



// Target input mappings
#define PIN_PAIR            36
#define PIN_PHOTOTRANSISTOR 18

// Target ouput mappings
#define LED_PIN 13

// Target PWM mappings
#ifdef PLATFORM_TARGET
    #define PWM_PIN_3V3 12
    #define PWM_PIN_5V  33
    #define PWM_PIN_12V 27
#endif
