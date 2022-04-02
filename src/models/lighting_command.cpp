#include "lighting_command.h"

String LightingCommand::toString()
{
    // First: encode lighting pattern
    String encodedString = String(pattern, HEX);
    // Encode the loop/clear bools
    encodedString += String(loop);
    encodedString += String(clear);
    // Encode our 32-bit integers stuff
    encodedString += String(startTime) + " ";
    encodedString += String(timeout) + " ";
    encodedString += String(frequency) + " ";
    encodedString += String(primaryColor) + " ";
    encodedString += String(secondaryColor);

    return encodedString;
}

int LightingCommand::hexChar2Int(char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    return -1;
}

uint32_t LightingCommand::rgbToUint(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t) r << 16) | ((uint32_t) g << 8) | ((uint32_t) b);
}