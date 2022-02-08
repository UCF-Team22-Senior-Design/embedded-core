#include "module.h"

class ReadyModule : Module
{
private:
    static int currentMenuPage;
public:
    static void onInitialize();
    static void onWake();
    static void onSleep();
    static void onUpdate();
    static void onInput(InputSource source, bool state);
    static void onWirelessEvent(NetworkMessage message);
};