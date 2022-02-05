#pragma once

#include <Arduino.h>
#include <painlessMesh.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN 2

enum LightingPattern
{
    OFF,
    BLINK_ALL,
    MARCHING_BLINK,
    // Add more patterns as they are developed
};

class LightingManager
{
private:
    // Pattern functions go here
    static void patternOff();
    static void patternBlinkAll();
    static void patternMarchingBlink();

    // Internal state information
    static LightingPattern currentLightingPattern;
    static bool loop;
    static uint32_t timeStart;
    static uint32_t timeOut;

    static void lightingUpdate();
    static Task taskLightingUpdate;

    // The actual lights
public:
    static void initialize(Scheduler *scheduler);

    static void stopPattern();
    static void startPattern();

    static void setColor(int r, int g, int b);
    static void setTimeout(uint32_t timeout);
    static void setPattern(LightingPattern pattern);

    static LightingPattern getCurrentPattern();
    static bool isPlaying();
};