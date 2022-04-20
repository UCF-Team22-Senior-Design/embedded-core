#include "lighting_command.h"

LightingCommand::LightingCommand(const char* lightingData)
{
    // First: Get our effect, loop, and clear information
    pattern = static_cast<LightingPattern>(hexChar2Int(lightingData[0]));
    loop = (lightingData[1] == '1');
    clear = (lightingData[2] == '1');

    // Next: Collect our start time, timeout, frequency, primary, and
    // secondary color data using the delimiters in the string (spaces)
    std::vector<String> splitString;

    // While we're not at the end of the string...
    lightingData += 3;
    splitString.push_back(String());
    while((*lightingData) != 0)
    {
        if((*lightingData) == ' ')
        {
            Serial.printf("Huzza! I have finished another string! Here it be! %s\n", splitString.back().c_str());
            splitString.push_back(String());
        }
        else
        {
            splitString.back() += String(static_cast<char>((*lightingData)));
        }
        
        // Increment the pointer
        lightingData++;
    }

    startTime = static_cast<uint32_t>(strtoul(splitString[0].c_str(), NULL, 10));
    timeout = strtoul(splitString[1].c_str(), NULL, 10);
    frequency = strtoul(splitString[2].c_str(), NULL, 10);
    primaryColor = static_cast<uint32_t>(strtoul(splitString[3].c_str(), NULL, 10));
    secondaryColor = static_cast<uint32_t>(strtoul(splitString[4].c_str(), NULL, 10));

    // Final Format:
    // Lighting Pattern
    // |Loop
    // || Clear
    // |||   Start Time
    // |||   |    Timeout
    // |||   |    |    Frequency
    // |||   |    |    |    Primary Color
    // |||   |    |    |    |   Secondary Color
    // x00cccc cccc cccc cccc cccc
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