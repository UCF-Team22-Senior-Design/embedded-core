#pragma once

#include <Arduino.h>
#include <painlessMesh.h>
#include <Adafruit_NeoPixel.h>

#include "../models/pinouts.h"

#define PIXELS_PER_SEGMENT 3
#define NUM_SEGMENTS 4
#define NUM_PIXELS PIXELS_PER_SEGMENT * NUM_SEGMENTS

enum LightingPattern
{
    STATIC,
    BLINK_ALL,
    MARCHING_BLINK,
    // Add more patterns as they are developed
};

class LightingManager
{
private:
    // Pattern functions go here
    static void patternStatic();
    static void patternBlinkAll();
    static void patternMarchingBlink();

    // Internal state information
    static LightingPattern currentLightingPattern;
    static bool loop;
    static bool clearOnStop;
    static unsigned long timeStart;
    static unsigned long timeOut;
    static uint32_t primaryColor;
    static uint32_t secondaryColor;

    static void lightingUpdate();
    static Task taskLightingUpdate;

    // The actual lights
    static Adafruit_NeoPixel pixels;
public:
    static void initialize(Scheduler *scheduler);

    static void stopPattern();
    static void startPattern();

    static void setPrimaryColor(uint8_t r, uint8_t g, uint8_t b);
    static void setSecondaryColor(uint8_t r, uint8_t g, uint8_t b);
    static void setTimeout(unsigned long timeout);
    static void setPattern(LightingPattern pattern);
    static void setClearOnStop(bool newClearOnStop);
    static void setLoop(bool loopValue);

    static LightingPattern getCurrentPattern();
    static bool isPlaying();
};