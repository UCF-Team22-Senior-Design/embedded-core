#pragma once

#include <painlessMesh.h>

class ConfigurationData
{
public:
    // Data that is saved/loaded
    float volume;

    // Public-facing methods
    JsonObject serialize();
    ConfigurationData(JsonObject data);
    ConfigurationData() : 
        volume{ 0.125 } {};
};