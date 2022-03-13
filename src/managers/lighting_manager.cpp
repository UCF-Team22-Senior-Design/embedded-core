#include "lighting_manager.h"

LightingPattern LightingManager::currentLightingPattern = LightingPattern::STATIC;
bool LightingManager::loop = false;
bool LightingManager::clearOnStop = false;
unsigned long LightingManager::timeStart = 0;
unsigned long LightingManager::timeOut = 0;
unsigned long LightingManager::effectFrequency = 100;
uint32_t LightingManager::primaryColor = 0;
uint32_t LightingManager::secondaryColor = 0;

Adafruit_NeoPixel LightingManager::pixels(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

Task LightingManager::taskLightingUpdate(TASK_MILLISECOND * 50, TASK_FOREVER, &lightingUpdate);

/**
 * @brief Establish the hardware control for our LED strips
 *
 * @param scheduler the user scheduler, which is used to register the lighting
 *                  update tasks
 */
void LightingManager::initialize(Scheduler *scheduler)
{
    // Ensure this runs only once.
    static bool hasBeenInitialized = false;
    if (hasBeenInitialized)
        return;
    hasBeenInitialized = true;

    // Initialize the LEDs
    pixels.begin();
    pixels.clear();

    // Set our default values for loop / timeStart/End/lightingPattern
    loop = false;
    clearOnStop = true;
    timeStart = 0;
    timeOut = 0;
    primaryColor = 0;
    secondaryColor = 0;
    currentLightingPattern = LightingPattern::STATIC;

    // Assign our task to the taskScheduler
    scheduler->addTask(taskLightingUpdate);

    Serial.println("Lighting manager initialized");
}

/**
 * @brief Stops the current pattern, if it is playing. If clearOnStop has been
 *        set, then the strip is also cleared.
 *
 */
void LightingManager::stopPattern()
{
    // Disable the task
    taskLightingUpdate.disable();

    // If we're to clear the lights on stop, then do so
    if (clearOnStop)
        pixels.clear();
}

/**
 * @brief Starts the currently set pattern
 *
 */
void LightingManager::startPattern()
{
    // Track when playback begins
    timeStart = millis();

    // Restart the task
    taskLightingUpdate.restart();
}

/**
 * @brief Sets the primary color to be used during patterns, given R, G, and B
 *        values.
 *
 * @param r An 8-bit red value
 * @param g An 8-bit green value
 * @param b An 8-bit blue value
 */
void LightingManager::setPrimaryColor(uint8_t r, uint8_t g, uint8_t b)
{
    primaryColor = pixels.Color(r, g, b);
}

/**
 * @brief Sets the secondary color to be used during patterns, given R, G, and B
 *        values.
 *
 * @param r An 8-bit red value
 * @param g An 8-bit green value
 * @param b An 8-bit blue value
 */
void LightingManager::setSecondaryColor(uint8_t r, uint8_t g, uint8_t b)
{
    secondaryColor = pixels.Color(r, g, b);
}

/**
 * @brief Sets the timeout for the current pattern - when the pattern is
 *        disabled. If loop is set, then this is ignored.
 *
 * @param timeout The amount of time, in milliseconds, the pattern should run
 *                for.
 */
void LightingManager::setTimeout(unsigned long timeout)
{
    timeOut = timeout;
}

/**
 * @brief Sets the pattern to be played. Can technically be changed during
 *        playback, but it might produce unexpected results.
 *
 * @param pattern The new pattern for the lighting program to follow
 */
void LightingManager::setPattern(LightingPattern pattern)
{
    currentLightingPattern = pattern;
}

/**
 * @brief Sets whether the lighting strip is cleared when the current lighting
 *        pattern is stopped. If true, the lighting strip will be cleared /
 *        turned off. If false, the lighting strip will stay as it was.
 *
 * @param newClearOnStop
 */
void LightingManager::setClearOnStop(bool newClearOnStop)
{
    clearOnStop = newClearOnStop;
}

/**
 * @brief If loop is true, then the system will repeat a pattern indefenitely
 *        until told to stop, ignoring the timeOut value. If false, system will
 *        rely on the timeout value.
 *
 * @param loopValue
 */
void LightingManager::setLoop(bool loopValue)
{
    loop = loopValue;
}

/**
 * @brief Returns the current lighting pattern of the lighting strip
 *
 * @return LightingPattern
 */
LightingPattern LightingManager::getCurrentPattern()
{
    return currentLightingPattern;
}

/**
 * @brief Returns whether the lightingStrip is currently playing any given
 *        pattern
 */
bool LightingManager::isPlaying()
{
    return taskLightingUpdate.isEnabled();
}

void LightingManager::setFrequency(unsigned long frequency)
{
    effectFrequency = frequency;
}

/**
 * @brief An internal function used to update the lighting strip each light
 *        update, and select which lighting pattern program is used based upon
 *        variou settings.
 *
 */
void LightingManager::lightingUpdate()
{
    // This bad boy manages update lighting things

    // First, check if we should be playing
    if (!loop && millis() - timeStart > timeOut)
    {
        // If we're over time, then go through and shut down the pattern
        // If we're supposed to clear on top, then clear the pixel strip
        if (clearOnStop)
            pixels.clear();

        // Disable our task
        taskLightingUpdate.disable();

        // End
        return;
    }

    // Next, perform any/all lighting updates required using the appropriate
    // lighting program
    switch (currentLightingPattern)
    {
    case STATIC:
        patternStatic();
        break;
    case BLINK_ALL:
        patternBlinkAll();
        break;
    case MARCHING_BLINK:
        patternMarchingBlink();
        break;
    default:
        pixels.clear();
        break;
    }
}

unsigned long LightingManager::arrayToUL(const char *byteArray)
{
    unsigned long intToReturn = (byteArray[0]) << 24;
    intToReturn |= byteArray[1] << 16;
    intToReturn |= byteArray[2] << 8;
    intToReturn |= byteArray[3];

    return intToReturn;
}

uint32_t LightingManager::arrayToUint(const char *byteArray)
{
    return static_cast<uint32_t>(arrayToUL(byteArray));
}

int LightingManager::hexChar2Int(char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    return -1;
}

void LightingManager::setEffect(const char *effectCode)
{
    /* -- Effect String Encoding -- */
    // This is designed to be an easy way for a controller to send a known effect code to a
    // target, which then follows it. It encodes all the parameters an effect can have into
    // a simple, fixed-length string, which makes it easy to command.

    /* Things to encode:
     * - Lighting Pattern (hex-coded single character, zero-indexed to the enum)
     * - Loop (bool, 1/0, single character)
     * - Clear (bool, 1/0, single character)
     * - Start Time (4-byte array, microseconds, uint32_t)
     * - Timeout (4-byte array, milliseconds, unsigned long)
     * - Frequency (4-byte array, unsigned long)
     * - Primary color (4-byte array, uint32_t)
     * - Secondary color (4-byte array, uint32_t)
     */

    // Final Format:
    // Lighting Pattern
    // | Loop
    // | | Clear
    // | | |    Start Time
    // | | |    |    Timeout
    // | | |    |    |    Frequency
    // | | |    |    |    |    Primary Color
    // | | |    |    |    |    |   Secondary Color
    // x|0|0|cccc|cccc|cccc|cccc|cccc

    // First - Stop whatever lighting effect is currently going on
    taskLightingUpdate.disable();
    if (clearOnStop)
        pixels.clear();

    // Next, assign the known things
    currentLightingPattern = static_cast<LightingPattern>(hexChar2Int(effectCode[0]));
    loop = (effectCode[1] == '1');
    clearOnStop = (effectCode[2] == '1');
    uint32_t startTime = arrayToUL(effectCode + 3);
    timeOut = arrayToUL(effectCode + 7);
    effectFrequency = arrayToUL(effectCode + 11);
    primaryColor = arrayToUint(effectCode + 15);
    secondaryColor = arrayToUint(effectCode + 19);

    // Have the system start with a delay
    uint32_t nodeTime = NetworkManager::getNodeTime();
    uint32_t timeDelay = startTime - nodeTime;
    if (timeDelay <= 50000)
    {
        // If we're to start the effect within this expected frame, start it immediately
        timeStart = millis();
        taskLightingUpdate.restart();
        return;
    }

    // Otherwise, start with a delay
    unsigned long timeDelayMillis = timeDelay / 1000;
    timeStart = millis() + timeDelayMillis;
    taskLightingUpdate.restartDelayed(timeDelayMillis);
}

//// ---------- Patterns go below ----------------- ///
void LightingManager::patternStatic()
{
    // Maintains the static color on the lighting strip, not re-setting it until
    // the staticColor changes, saving some time/cycles.
    static uint32_t lastStaticColor = 0;
    if (lastStaticColor != primaryColor)
    {
        lastStaticColor = primaryColor;
        pixels.clear();
        for (int i = 0; i < NUM_PIXELS; i++)
        {
            pixels.setPixelColor(i, primaryColor);
        }
        pixels.show();
    }
}

void LightingManager::patternBlinkAll()
{
    // PatternBlinkAll: Blinks all lights on / off every second, between primary
    // and secondary colors
    pixels.clear();

    bool primaryOrSecondary = ((((millis() - timeStart) / effectFrequency) % 2) == 0);
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        pixels.setPixelColor(i, primaryOrSecondary ? primaryColor : secondaryColor);
    }

    pixels.show();
}

void LightingManager::patternMarchingBlink()
{
    // PatternMarchingBlink - incrementally switch each LED between primary and
    // secondary colors, per-segment.
    static int marchIndex = 0;

    // We should march every other marchPeriod
    bool shouldMarch = ((((millis() - timeStart) / effectFrequency) % 2) == 0);
    if (!shouldMarch)
        return;

    // Clear the strip
    pixels.clear();

    // Alternate between the primary and secondary colors every time the march
    // returns to the zero index
    bool stepColor = ((marchIndex / PIXELS_PER_SEGMENT) % 2 == 0);
    // For each segment...
    for (int i = 0; i < NUM_SEGMENTS; i++)
    {
        // For each pixel...
        for (int j = 0; j < PIXELS_PER_SEGMENT; j++)
        {
            // Calculate the pixel index
            int pixelIndex = i * PIXELS_PER_SEGMENT + j;

            // If we're before the march index, set the color one way
            if (j <= (marchIndex % PIXELS_PER_SEGMENT))
                pixels.setPixelColor(pixelIndex, stepColor ? primaryColor : secondaryColor);
            // If we're after the march index, set the color the other way
            else
                pixels.setPixelColor(pixelIndex, stepColor ? secondaryColor : primaryColor);
        }
    }

    // Increment our march index
    marchIndex++;
    if (marchIndex > 5)
        marchIndex = 0;

    pixels.show();
}