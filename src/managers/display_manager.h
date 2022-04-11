#pragma once

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "../models/pinouts.h"

#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT  64
#define SCREEN_ADDRESS 0x3D
#define FONT_WIDTH     5
#define FONT_HEIGHT    8
#define CHARS_PER_LINE SCREEN_WIDTH / FONT_WIDTH

class DisplayManager
{
private:
    static Adafruit_SSD1306 display;
    static void printCentered(const char* text, int initialY, bool underlined = false);
    static void printUnderlined(const char* text, int x, int y);
    static void printOptionLabels(const char* option1, const char* option2);
    static void printTitle(const char* title);

public:
    // Basic screen functions
    static void drawSimpleScreen(const char* text, int textSize = 1);
    static void drawBasicScreen(const char* option1, const char* option2, const char* title, const char* helperText);
    static void drawTopCornerOverlay(const char* text);
    static void drawFourOptionSelectScreen(const char* title, const char* option1, const char* option2, const char* option3, const char* option4, int activeOption, const char* printOption1, const char* printOption2);

    static void initialize();
};