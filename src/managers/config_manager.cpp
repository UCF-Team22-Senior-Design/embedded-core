#include "config_manager.h"

ConfigurationData ConfigManager::configData;

/**
 * @brief This function resets all the configuration data of the device, then reboots the device. This ensures that
 *        the system reloads with all appropriate default settings
 */
void ConfigManager::resetData()
{
    // Set our configData to default
    configData = ConfigurationData();

    // Inform the debugger <3
    Serial.println("<ConfigManager> Resetting and rebooting all data on device");

    // Save this data
    saveData();

    // Restart the device
    ESP.restart();
}

/**
 * @brief Saves our current configuration data to our config file, located at CONFIG_FILE_PATH
 * 
 */
void ConfigManager::saveData()
{
    // Take our configData object... transform it into a jsonObject
    // and then save it to spiffs.

    // First: Attempt to open the file
    File file = SPIFFS.open(CONFIG_FILE_PATH, FILE_WRITE);
    if (!file)
    {
        Serial.println("<ConfigManager> Failed to open configuration file for writing");
        return;
    }
    else
    {
        Serial.printf("<ConfigManager> Opened file at path %s\n", CONFIG_FILE_PATH);
    }

    // Second: Write to the file using ArduinoJson's serialization stuff
    DynamicJsonDocument doc = configData.serialize();

    // Let's see if this json object has anything in it
    if (serializeJson(doc, file))
    {
        Serial.println("<ConfigManager> Saved configuration data to fs");
    }
    else
    {
        Serial.println("<ConfigManager> Failed to save configuration data");
    }

    file.close();
}

/**
 * @brief Loads our configuration data from CONFIG_FILE_PATH, and assigns it to the configData member
 * 
 */
void ConfigManager::loadData()
{
    // First: Attempt to open the file
    File file = SPIFFS.open(CONFIG_FILE_PATH);
    if (!file || file.isDirectory())
    {
        // We were unable to open the file to load configuration data
        Serial.println("<ConfigManager> Failed to open configuration file for reading");
        return;
    }

    DynamicJsonDocument doc(1024);

    DeserializationError error = deserializeJson(doc, file);

    if (error)
    {
        Serial.println("<ConfigManager> Error occured during deserialization");
        Serial.printf("<ConfigManager> Error: %s", error.c_str());
        Serial.printf("<ConfigManager> File Content: %s\n", file.readString().c_str());
        file.close();
        return;
    }

    // We have loaded the file! Parse it!
    try
    {
        configData = ConfigurationData(doc.as<JsonObject>());
        // Data is now loaded!
        Serial.println("<ConfigManager Configuration data loaded from flash storage successfully");
    }
    catch (const std::exception e)
    {
        Serial.printf("<ConfigManager> Failed to deserialie configuration data: %s\n", e.what());
    }

    file.close();
}