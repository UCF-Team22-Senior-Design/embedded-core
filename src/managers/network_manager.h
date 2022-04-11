#pragma once

#include <unordered_map>

// Dependencies for PainlessMesh
#include <WiFi.h>
#include <SPIFFS.h>
#include <Update.h>
#include <painlessMesh.h>

#include "../models/network_message.h"

#define DEFAULT_MESH_PREFIX   "LTGPairingNet"
#define DEFAULT_MESH_PASSWORD "t9WNq8Yx^9vgJKG1"
#define MESH_PORT             5555

typedef void(*NetworkMessageCallback)(NetworkMessage message);

class NetworkManager
{
private:
    static painlessMesh mesh;

    static std::unordered_map<std::string, std::vector<NetworkMessageCallback>> callbacks;

    static void receivedMessage(uint32_t from, String &message);
public:
    NetworkManager();

    static void update();

    static void initialize(Scheduler* scheduler);
    static void sendMessage(NetworkMessage message);
    static void sendMessage(NetworkMessage message, uint32_t destination);
    static void registerCallback(NetworkMessageCallback callback, String tagFilter = "NONE");
    static void deregisterCallback(NetworkMessageCallback callback, String tagFilter);

    static int getNumNodes();

    static uint32_t getNodeTime();
    static uint32_t getNodeID();
};