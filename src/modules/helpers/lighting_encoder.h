#pragma once

#include <Arduino.h>

class LightingEncoder
{
private:
    static String uintToString(uint32_t input);
    static String ulonToString(unsigned long input);
public:
    static uint32_t rgbToUint(uint8_t r, uint8_t g, uint8_t b);
    static String encodeLightingEffect(char lightingPattern, bool loop, bool clearOnStop, uint32_t startTime, unsigned long timeout, unsigned long effectFrequency, uint32_t primaryColor, uint32_t secondaryColor);
};