#pragma once

#include "../managers/input_manager.h"
#include "../managers/network_manager.h"

class Module 
{
public:
    static void onInitialize() {};
    static void onWake() {};
    static void onSleep() {};
    static void onUpdate() {};
    static void onInput(InputSource source, bool state) {};
    static void onWirelessEvent(NetworkMessage message) {};
};