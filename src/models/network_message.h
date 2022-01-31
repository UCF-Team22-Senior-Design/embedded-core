#pragma once

#include <string>

#include <Arduino.h>
#include <ArduinoJson.h>

class NetworkMessage 
{
private:
    uint32_t sender;
    uint32_t timestamp;
    String tag;
    String data;
public:
    NetworkMessage(uint32_t from, String payload);
    NetworkMessage(String outTag, String outData, uint32_t currentTime);
    String toString();

    uint32_t getSender();
    uint32_t getTimestamp();
    String getTag();
    String getData();
};