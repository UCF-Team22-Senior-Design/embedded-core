#pragma once

#include <painlessMesh.h>
#include <set>

class ConfigurationData
{
public:
    // Data that is saved/loaded

    // Public-facing methods
    DynamicJsonDocument serialize();
    ConfigurationData(JsonObject data);
    // Default Constructor - fill with default assigned values
    ConfigurationData() {};
};