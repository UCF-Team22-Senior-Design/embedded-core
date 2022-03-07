#include "config_manager.h"

ConfigurationData ConfigManager::configData;

void ConfigManager::saveData()
{
    // Take our configData object... transform it into a jsonObject
    // and then save it to spiffs.

    // First: Attempt to open the file
    File file = SPIFFS.open(CONFIG_FILE_PATH, FILE_WRITE);
    if(!file)
    {
        Serial.println("[ConfigManager] Failed to open configuration file for writing");
        return;
    }
    else
    {
        Serial.printf("[ConfigManager] Opened file at path %s\n", CONFIG_FILE_PATH);
    }

    // Second: Write to the file using ArduinoJson's serialization stuff
    if(serializeJson(configData.serialize(), file))
    {
        Serial.println("[ConfigManager] Saved configuration data to fs");
    } 
    else 
    {
        Serial.println("[ConfigManager] Failed to save configuration data");
    }

    file.close();
}

void ConfigManager::loadData()
{
    // First: Attempt to open the file
    File file = SPIFFS.open(CONFIG_FILE_PATH);
    if(!file || file.isDirectory())
    {
        // We were unable to open the file to load configuration data
        Serial.println("[ConfigManager] Failed to open configuration file for reading");
        return;
    }

    DynamicJsonDocument doc(1024);

    DeserializationError error = deserializeJson(doc, file);

    if(error)
    {
        Serial.println("[ConfigManager] Error occured during deserialization");
        Serial.printf("[ConfigManager] Error: ");
        Serial.println(error.c_str());
        Serial.printf("[ConfigManager] File Content: %s\n", file.readString().c_str());
    }

    file.close();
}