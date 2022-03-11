#pragma once
#include <painlessMesh.h>

#include "FS.h"
#include "SPIFFS.h"

#include "../models/configuration_data.h"

#define CONFIG_FILE_PATH "/cfg/config.json"

class ConfigManager
{
public:
    // Load long-lasting configuration data to/from file
    static void saveData();
    static void loadData();
    static void resetData();

    static ConfigurationData configData;
}; 