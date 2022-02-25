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
    static void printCentered(String text, int initialY, bool underlined = false);
    static void printUnderlined(String text, int x, int y);
    static void printOptionLabels(String option1, String option2);
    static void printTitle(String title);

public:
    // Basic screen functions
    static void drawSimpleScreen(String text, int textSize = 1);
    static void drawBasicScreen(String option1, String option2, String title, String helperText);
    static void drawFourOptionSelectScreen(String title, String option1, String option2, String option3, String option4, int activeOption, String printOption1, String printOption2);

    static void initialize();
};