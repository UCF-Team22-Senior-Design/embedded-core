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

class DisplayManager
{
private:
    static Adafruit_SSD1306 display;

public:
    // Basic screen functions
    static void drawSimpleScreen(String text, int textSize = 1);
    static void initialize();
}