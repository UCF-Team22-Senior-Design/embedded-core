#pragma once

#include <painlessMesh.h>
#include <set>

class ConfigurationData
{
public:
    // Data that is saved/loaded
    float volume;
    std::set<uint32_t> targets;

    // Public-facing methods
    DynamicJsonDocument serialize();
    ConfigurationData(JsonObject data);
    // Default Constructor - fill with default assigned values
    ConfigurationData() : 
        volume{ 0.125 }, targets {} {};
};