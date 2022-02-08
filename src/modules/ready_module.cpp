#include "ready_module.h"

int ReadyModule::currentMenuPage = 0;

void ReadyModule::onInitialize()
{
    // Reset our menu page to zero
    currentMenuPage = 0;
    
}

void ReadyModule::onWake()
{
    // Reset our menu page to zero;
    currentMenuPage = 0;
}

void ReadyModule::onSleep()
{
    // Put to sleep whatever we have been running
}

void ReadyModule::onUpdate()
{
    // Runs every "frame" - perform fixed update things, such as checking inputs,
    // updating screen, etc.
}

void ReadyModule::onInput(InputSource source, bool state)
{
    // Every time an input is pressed, this is called.
}

void ReadyModule::onWirelessEvent(NetworkMessage message)
{
    // Every time a wireless event happens, do something.
}