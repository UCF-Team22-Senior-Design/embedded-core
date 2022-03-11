#pragma once

#include <painlessMesh.h>

class ConfigurationData
{
public:
    // Data that is saved/loaded
    uint32_t controller;

    // Public-facing methods
    DynamicJsonDocument serialize();
    ConfigurationData(JsonObject data);
    // Default Constructor - fill with default assigned values
    ConfigurationData() : controller{0} {};
};