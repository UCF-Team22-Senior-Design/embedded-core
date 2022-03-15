#pragma once

#include <string>

#include <Arduino.h>
#include <ArduinoJson.h>

#define MESSAGE_TAG_PAIR_REQUEST      "PAIR_REQUEST"
#define MESSAGE_TAG_PAIR_REJECT       "PAIR_REJECT"
#define MESSAGE_TAG_PAIR_ACCEPTED     "PAIR_ACCEPTED"
#define MESSAGE_TAG_PAIR_COMPLETE     "PAIR_COMPLETE"
#define MESSAGE_TAG_TARGET_HIT        "TARGET_HIT"
#define MESSAGE_TAG_TARGET_IGNITE     "TARGET_IGNITE"
#define MESSAGE_TAG_TARGET_EXTINGUISH "TARGET_EXTINGUISH"
#define MESSAGE_TAG_TARGET_ON_HIT     "TARGET_ON_HIT"
#define MESSAGE_TAG_GAME_START        "GAME_START"
#define MESSAGE_TAG_GAME_END          "GAME_END"

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