#include "configuration_data.h"

DynamicJsonDocument ConfigurationData::serialize()
{
    // Create a JSONObject/Document from our data that
    // we have.

    DynamicJsonDocument dataDoc(1024);

    dataDoc["audio"] = volume;

    serializeJsonPretty(dataDoc, Serial);

    // Return a reference to the document
    return dataDoc;
}

ConfigurationData::ConfigurationData(JsonObject data)
{
    // We have a data, loaded from memory.
    // Load our data back from it
    volume = data["audio"];
}