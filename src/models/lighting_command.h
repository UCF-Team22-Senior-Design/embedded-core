#pragma once

#include <Arduino.h>
#include <vector>

enum LightingPattern
{
    STATIC,
    BLINK_ALL,
    MARCHING_BLINK,
    // Add more patterns as they are developed
};

class LightingCommand
{
public:
    // Lighting Data

    LightingPattern pattern;
    bool loop;
    bool clear;
    uint32_t startTime;
    unsigned long timeout;
    unsigned long frequency;
    uint32_t primaryColor;
    uint32_t secondaryColor;

    static int hexChar2Int(char data);
    static uint32_t rgbToUint(uint8_t r, uint8_t g, uint8_t b);

    String toString();

    // A default lighting command - static "off"
    LightingCommand() :
        pattern{LightingPattern::STATIC}, loop{false}, clear{false}, startTime{0}, timeout{0}, 
        frequency{0}, primaryColor{0}, secondaryColor{0} {};

    // A custom lighting command
    LightingCommand(LightingPattern pattern, bool loop, bool clear, uint32_t startTime,
        unsigned long timeout, unsigned long frequency, uint32_t primaryColor, uint32_t secondaryColor) :
        pattern{pattern}, loop{loop}, clear{clear}, startTime{startTime}, timeout{timeout}, 
        frequency{frequency}, primaryColor{primaryColor}, secondaryColor{secondaryColor} {};
};