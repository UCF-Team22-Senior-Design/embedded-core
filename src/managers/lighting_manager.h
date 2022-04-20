#pragma once

#include <Arduino.h>
#include <painlessMesh.h>
#include <Adafruit_NeoPixel.h>

#include "../models/pinouts.h"
#include "../models/lighting_command.h"
#include "managers/input_manager.h"
#include "network_manager.h"

#define PIXELS_PER_SEGMENT 3
#define NUM_SEGMENTS 4
#define NUM_PIXELS PIXELS_PER_SEGMENT * NUM_SEGMENTS

typedef void(*LightingEndedCallback)();

class LightingManager
{
private:
    // Pattern functions go here
    static void patternStatic();
    static void patternBlinkAll();
    static void patternMarchingBlink();

    // Internal state information
    static LightingCommand* activeLightingCommand;
    static LightingCommand  standbyLightingCommand;
    static LightingCommand  onHitLightingCommand;

    /*
    static LightingPattern currentLightingPattern;
    static bool loop;
    static bool clearOnStop;
    static unsigned long timeStart;
    static unsigned long timeOut;
    static unsigned long effectFrequency;
    static uint32_t primaryColor;
    static uint32_t secondaryColor;
    */

    static unsigned long timeStart; 
    static bool inStandby;
    static void onHitHandler(InputSource _, bool state);

    static void lightingUpdate();
    static Task taskLightingUpdate;

    // The actual lights
    static Adafruit_NeoPixel pixels;
public:
    static void initialize(Scheduler *scheduler);

    static void stopPattern();
    static void startPattern();
    static void resetLightingState();

    static void setStandbyLightingCommand(LightingCommand newStandbyCommand);
    static void setOnHitLightingCommand(LightingCommand newOnHitCommand);
    
    static LightingPattern getCurrentPattern();
    static bool isPlaying();
};