#include "configuration_data.h"

DynamicJsonDocument ConfigurationData::serialize()
{
    // Create a JSONObject/Document from our data that
    // we have.

    DynamicJsonDocument dataDoc(1024);

    // Implement our data to save
    dataDoc["controller"] = controller;


    serializeJsonPretty(dataDoc, Serial);

    // Return a reference to the document
    return dataDoc;
}

ConfigurationData::ConfigurationData(JsonObject data)
{
    // We have a data, loaded from memory.
    // Load our data back from it
    controller = static_cast<uint32_t>(data["controller"]);
}