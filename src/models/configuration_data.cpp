#include "configuration_data.h"

DynamicJsonDocument ConfigurationData::serialize()
{
    // Create a JSONObject/Document from our data that
    // we have.

    DynamicJsonDocument dataDoc(1024);

    dataDoc["audio"] = volume;

    // Transform the vector of targets into an array for storage
    JsonArray targetArray = dataDoc["targets"].to<JsonArray>();
    for(auto target : targets)
    {
        targetArray.add(target);
    }

    serializeJsonPretty(dataDoc, Serial);

    // Return a reference to the document
    return dataDoc;
}

ConfigurationData::ConfigurationData(JsonObject data)
{
    // We have a data, loaded from memory.
    // Load our data back from it
    volume = data["audio"];

    // Load our target information
    JsonArray targetsArray = data["targets"];
    targets.clear();
    for(JsonVariant targetData : targetsArray)
    {
        targets.insert(static_cast<uint32_t>(targetData.as<unsigned long>()));
    }
}