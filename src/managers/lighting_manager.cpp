#include "lighting_manager.h"

LightingCommand* LightingManager::activeLightingCommand;
LightingCommand  LightingManager::standbyLightingCommand;
LightingCommand  LightingManager::onHitLightingCommand;

Adafruit_NeoPixel LightingManager::pixels(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

bool LightingManager::inStandby = true;
unsigned long LightingManager::timeStart;

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

    // Set our default values for our lighting patterns
    standbyLightingCommand = LightingCommand(LightingPattern::STATIC, false, false, 0, 0, 0, 
        LightingCommand::rgbToUint(0, 0, 0), LightingCommand::rgbToUint(0, 0, 0));
    onHitLightingCommand = LightingCommand(LightingPattern::STATIC, false, false, 0, 100, 0, 
        LightingCommand::rgbToUint(255, 255, 255), LightingCommand::rgbToUint(0, 0, 0));

    // Set our active command
    activeLightingCommand = &standbyLightingCommand;

    // Set our standby status
    inStandby = true;

    // Register our input callback
    InputManager::registerInputCallback(onHitHandler, InputSource::PHOTOTRANSISTOR);

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
    if (activeLightingCommand->clear)
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

    // Update the frequency of update, if relevant
    if(activeLightingCommand->frequency != 0)
    {
        taskLightingUpdate.setInterval(TASK_MILLISECOND * activeLightingCommand->frequency);
    }
    else
    {
        taskLightingUpdate.setInterval(TASK_MILLISECOND * 50);
    }

    // Restart the task
    taskLightingUpdate.restart();
}

/**
 * @brief Returns whether the lightingStrip is currently playing any given
 *        pattern
 */
bool LightingManager::isPlaying()
{
    return taskLightingUpdate.isEnabled();
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
    if (!activeLightingCommand->loop && millis() - timeStart > activeLightingCommand->timeout)
    {
        // If we're over time, then go through and shut down the pattern
        // If we're supposed to clear on top, then clear the pixel strip
        if (activeLightingCommand->clear)
            pixels.clear();

        // Disable our task
        taskLightingUpdate.disable();

        // If we're the "onHit" command, shift back to the standby command
        if(!inStandby)
        {
            activeLightingCommand = &standbyLightingCommand;
            inStandby = true;
        }
        
        // Reset our lighting task
        startPattern();

        // End
        return;
    }

    // Next, perform any/all lighting updates required using the appropriate
    // lighting program
    switch (activeLightingCommand->pattern)
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
/*
unsigned long LightingManager::stringToUL(const char *base64Data)
{
    Serial.printf("Byte array: %d %d %d %d\n", byteArray[0], byteArray[1], byteArray[2], byteArray[3]);
    unsigned long intToReturn = (byteArray[0]) << 24;
    intToReturn |= byteArray[1] << 16;
    intToReturn |= byteArray[2] << 8;
    intToReturn |= byteArray[3];

    return intToReturn;
}
/*
uint32_t LightingManager::arrayToUint(const char *byteArray)
{
    return static_cast<uint32_t>(arrayToUL(byteArray));
}*/
/*
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
/*
    // First - Stop whatever lighting effect is currently going on
    taskLightingUpdate.disable();
    if (clearOnStop)
        pixels.clear();

    Serial.printf("Hey, what's the size of our effectCode? %d\n", sizeof(effectCode));

    LightingCommand testLightingCommand("00017970709 0 100 111111111111 000000000000");
    Serial.printf("Test Lighting Command:\n");
    Serial.printf("\tPattern: %d\n", testLightingCommand.pattern);
    Serial.printf("\tLoop: %d\n", testLightingCommand.loop);
    Serial.printf("\tClear: %d\n", testLightingCommand.clear);
    Serial.printf("\tStart Time: %lu\n", testLightingCommand.startTime);
    Serial.printf("\tTimeout: %lu\n", testLightingCommand.timeout);
    Serial.printf("\tFrequency: %lu\n", testLightingCommand.frequency);
    Serial.printf("\tPrimary Color: %lu\n", testLightingCommand.primaryColor);
    Serial.printf("\tSecondary Color: %lu\n", testLightingCommand.secondaryColor);
/*
    // Next, assign the known things
    currentLightingPattern = static_cast<LightingPattern>(hexChar2Int(effectCode[0]));
    loop = (effectCode[1] == '1');
    clearOnStop = (effectCode[2] == '1');
    uint32_t startTime = arrayToUL(effectCode + 3);
    timeOut = arrayToUL(effectCode + 8);
    effectFrequency = arrayToUL(effectCode + 11);
    primaryColor = arrayToUint(effectCode + 15);
    secondaryColor = arrayToUint(effectCode + 19);

    std::vector<char> primaryColorAsRGB = uintToRGB(primaryColor);
    std::vector<char> secondaryColorAsRGB = uintToRGB(secondaryColor);

    Serial.printf("\tNet Time: %lu\n", NetworkManager::getNodeTime());
    Serial.printf("\tStart Time: %lu\n", startTime);
    Serial.printf("\tTime Out: %lu\n", timeOut);
    Serial.printf("\tFrequency: %lu\n", effectFrequency);
    Serial.printf("\tPrimary Color: %d %d %d\n", primaryColorAsRGB[0], primaryColorAsRGB[1], primaryColorAsRGB[2]);
    Serial.printf("\tSecondary Color: %d %d %d\n", secondaryColorAsRGB[0], secondaryColorAsRGB[1], secondaryColorAsRGB[2]);
    
    // Have the system start with a delay
    uint32_t nodeTime = NetworkManager::getNodeTime();
    uint32_t timeDelay = startTime - nodeTime;
    if (startTime < nodeTime || timeDelay <= 50000)
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
    */

   // Hey this function is kinda fucked right now
//}

void LightingManager::onHitHandler(InputSource _, bool state)
{
    // Only act when we are hit (going low)
    if(state) return;

    // When we're hit, stop the current pattern, shift into onHit, start playback
    stopPattern();
    inStandby = false;
    activeLightingCommand = &onHitLightingCommand;
    startPattern();
}

void LightingManager::setStandbyLightingCommand(LightingCommand newStandby)
{
    // Set the new command
    standbyLightingCommand = newStandby;
}

void LightingManager::setOnHitLightingCommand(LightingCommand newOnHit)
{
    // Set the new command
    onHitLightingCommand = newOnHit;
}


void LightingManager::resetLightingState()
{
    // Set the lighting state to standby
    stopPattern();
    inStandby = true;
    activeLightingCommand = &standbyLightingCommand;
    startPattern();
}

//// ---------- Patterns go below ----------------- ///
void LightingManager::patternStatic()
{
    // Maintains the static color on the lighting strip, not re-setting it until
    // the staticColor changes, saving some time/cycles.
    static uint32_t lastStaticColor = 0;
    if (lastStaticColor != activeLightingCommand->primaryColor)
    {
        lastStaticColor = activeLightingCommand->primaryColor;
        pixels.clear();
        for (int i = 0; i < NUM_PIXELS; i++)
        {
            pixels.setPixelColor(i, lastStaticColor);
        }
        pixels.show();
    }
}

void LightingManager::patternBlinkAll()
{
    // PatternBlinkAll: Blinks all lights on / off every second, between primary
    // and secondary colors
    pixels.clear();

    bool primaryOrSecondary = ((((millis() - timeStart) / activeLightingCommand->frequency) % 2) == 0);
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        pixels.setPixelColor(i, primaryOrSecondary ? activeLightingCommand->primaryColor : activeLightingCommand->secondaryColor);
    }

    pixels.show();
}

void LightingManager::patternMarchingBlink()
{
    // PatternMarchingBlink - incrementally switch each LED between primary and
    // secondary colors, per-segment.
    static int marchIndex = 0;

    // We should march every other marchPeriod
    bool shouldMarch = ((((millis() - timeStart) / activeLightingCommand->frequency) % 2) == 0);
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
                pixels.setPixelColor(pixelIndex, stepColor ? activeLightingCommand->primaryColor : activeLightingCommand->secondaryColor);
            // If we're after the march index, set the color the other way
            else
                pixels.setPixelColor(pixelIndex, stepColor ? activeLightingCommand->secondaryColor : activeLightingCommand->primaryColor);
        }
    }

    // Increment our march index
    marchIndex++;
    if (marchIndex > 5)
        marchIndex = 0;

    pixels.show();
}