#include "lighting_encoder.h"

String LightingEncoder::uintToString(uint32_t input)
{
    return ulonToString(static_cast<unsigned long>(input));
}

String LightingEncoder::ulonToString(unsigned long input)
{
    // Format our unsigned long into a string
    String compressed = String(static_cast<char>((input >> 24) & 0xFF));
    compressed +=       String(static_cast<char>((input >> 16) & 0xFF));
    compressed +=       String(static_cast<char>((input >> 8)  & 0xFF));
    compressed +=       String(static_cast<char>((input)       & 0xFF));

    return compressed;
}

uint32_t LightingEncoder::rgbToUint(uint8_t r, uint8_t g, uint8_t b)
{
    // Compress the r/g/b values into one 32-bit integer
    uint32_t compressed = (r << 16);
    compressed |= g << 8;
    compressed |= b;

    return compressed;
}

String LightingEncoder::encodeLightingEffect(char lightingPattern, bool loop, bool clearOnStop, uint32_t startTime, unsigned long timeout, unsigned long effectFrequency, uint32_t primaryColor, uint32_t secondaryColor)
{
    String encodedString = String(lightingPattern, HEX);
    encodedString += String(loop) + String(clearOnStop) + uintToString(startTime) + ulonToString(timeout) + ulonToString(effectFrequency) + uintToString(primaryColor) + uintToString(secondaryColor);
    return encodedString;
}
