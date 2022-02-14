#include "display_manager.h"

Adafruit_SSD1306 DisplayManager::display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

/**
 * @brief Initialize our display and outputs
 * 
 * @param scheduler The user scheduler
 */
void DisplayManager::initialize()
{
    // Initialize display hardware
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.printf("SSD1306 allocation failed");
    }
    else
    {
        drawSimpleScreen("Loading...", 2);
    }
}

/**
 * @brief Draws a blank simple screen with a line of text in the middle center
 * 
 * @param text The text to be displayed
 */
void DisplayManager::drawSimpleScreen(String text, int textSize)
{
    // Clear the display
    display.clearDisplay();

    // Set some text configuration things
    display.setTextSize(textSize);
    display.setTextColor(SSD1306_WHITE);
    display.setTextWrap(false);

    // Calculate vertical position for the text
    const int halfHeight = SCREEN_HEIGHT / 2;
    const int halfTextHeight = 4 * textSize; // Text Size 2 has a height of 10x16
    int16_t verticalPosition = halfHeight - halfTextHeight;

    // Calculate lateral position for the text
    int16_t finalX, finalY;
    uint16_t finalWidth, finalHeight;
    // Fun little helper function that calculates how big the final text will be
    display.getTextBounds(text.c_str(), 0, verticalPosition, 
        &finalX, &finalY, &finalWidth, &finalHeight);
    
    // The horizontal position should be the (width of the screen - width of text) / 2
    int calculatedHorizontalPosition = (SCREEN_WIDTH - finalWidth) / 2;

    // Move our cursor to that position
    display.setCursor(calculatedHorizontalPosition, verticalPosition);
    display.print(text.c_str());
    
    display.display();
}