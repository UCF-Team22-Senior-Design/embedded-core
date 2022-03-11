#pragma once

#include <painlessMesh.h>

class ConfigurationData
{
public:
    // Data that is saved/loaded
    float volume;

    // Public-facing methods
    DynamicJsonDocument serialize();
    ConfigurationData(JsonObject data);
    // Default Constructor - fill with default assigned values
    ConfigurationData() : 
        volume{ 0.125 } {};
};