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
        drawSimpleScreen("Loading...", 1);
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

    printCentered(text, verticalPosition, false);

    display.display();
}

void DisplayManager::drawBasicScreen(String option1, String option2, String title, String helperText)
{
    // Clear the display
    display.clearDisplay();

    printTitle(title);

    // Display helper text centered in the middle of the screen
    display.setCursor(0, (SCREEN_HEIGHT - FONT_HEIGHT) / 2);
    display.setTextWrap(true);
    display.print(helperText);

    // Display options
    printOptionLabels(option1, option2);

    display.display();
}

void DisplayManager::drawFourOptionSelectScreen(String title, String option1, String option2, String option3, String option4, int activeOption, String printOption1, String printOption2)
{
    // Clear the display
    display.clearDisplay();

    printTitle(title);

    // Print options with selected option underlined
    int offset = 8;
    printCentered(option1, (FONT_HEIGHT + 2) + offset, (activeOption == 1));
    printCentered(option2, (FONT_HEIGHT + 2) * 2 + offset, (activeOption == 2));
    printCentered(option3, (FONT_HEIGHT + 2) * 3 + offset, (activeOption == 3));
    printCentered(option4, (FONT_HEIGHT + 2) * 4 + offset, (activeOption == 4));

    // Display options
    printOptionLabels(printOption1, printOption2);

    display.display();
}

void DisplayManager::printCentered(String text, int initialY, bool underlined)
{
    // Calculate lateral position for the text
    int16_t finalX, finalY;
    uint16_t finalWidth, finalHeight;
    // Fun little helper function that calculates how big the final text will be
    display.getTextBounds(text.c_str(), 0, initialY,
                          &finalX, &finalY, &finalWidth, &finalHeight);

    // The horizontal position should be the (width of the screen - width of text) / 2
    int calculatedHorizontalPosition = (SCREEN_WIDTH - finalWidth) / 2;

    // Move our cursor to that position
    if (underlined)
    {
        printUnderlined(text, calculatedHorizontalPosition, initialY);
    }
    else
    {
        display.setCursor(calculatedHorizontalPosition, initialY);
        display.print(text.c_str());
    }
}

void DisplayManager::printUnderlined(String text, int x, int y)
{
    const int verticalOffset = 8;

    int16_t finalX, finalY;
    uint16_t finalWidth, finalHeight;

    display.getTextBounds(text.c_str(), x, y, &finalX, &finalY, &finalWidth, &finalHeight);

    display.setCursor(x, y);
    display.print(text.c_str());

    display.drawFastHLine(finalX, finalY + verticalOffset, finalWidth, SSD1306_WHITE);
}

void DisplayManager::printOptionLabels(String option1, String option2)
{
    // Display first option in the bottom left of the screen
    display.setCursor(0, SCREEN_HEIGHT - 8);
    display.print(option1.c_str());

    // Display second option in the bottom right of the screen
    display.setCursor(SCREEN_WIDTH - option2.length() * FONT_WIDTH - 8, SCREEN_HEIGHT - 8);
    display.print(option2.c_str());
}

void DisplayManager::printTitle(String title)
{
    // Display title centered and underlined at the top of the screen
    display.setCursor(0, 0);
    printCentered(title, 0, true);
}