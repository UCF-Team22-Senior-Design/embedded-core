#include "configuration_data.h"

JsonObject ConfigurationData::serialize()
{
    // Create a JSONObject/Document from our data that
    // we have.

    DynamicJsonDocument dataDoc(1024);

    dataDoc["audio"] = volume;

    // Return a reference to the document
    return dataDoc.as<JsonObject>();
}

ConfigurationData::ConfigurationData(JsonObject data)
{
    // We have a data, loaded from memory.
    // Load our data back from it
    volume = data["audio"];
}