#include "laser_manager.h"

// Static variables of our class must be initialized in the .cpp file - so, we
// initialize our stuff here, even if it has an empty/default constructor.
//
// C++ is stupid sometimes.
bool LaserManager::hasBeenInitialized = false;

/**
 * @brief A function to run once at the start of execution, which establishes
 *        the necessary callbacks for our laser manager. A static
 *        variable keeps track of whether it has been initalized or not, so you
 *        cannot call this twice.
 */
void LaserManager::initialize()
{
    // Check to see if we've been initialized already.
    if(hasBeenInitialized) return;

    // Register the laser pins as output
    pinMode(PIN_LASER1, OUTPUT);
    pinMode(PIN_LASER2, OUTPUT);
    pinMode(PIN_LASER3, OUTPUT);

    // Set the laser's state to off
    digitalWrite(PIN_LASER1, 0);
    digitalWrite(PIN_LASER2, 0);
    digitalWrite(PIN_LASER3, 0);

    // Register a callback for the input manager, listening to the trigger
    InputManager::registerInputCallback(LaserManager::triggerCallback, InputSource::BUTTON_TRIGGER);

    // Initialization complete
    hasBeenInitialized = true;
}

void LaserManager::triggerCallback(InputSource source, bool state)
{
    digitalWrite(PIN_LASER1, !state);
    digitalWrite(PIN_LASER2, !state);
    digitalWrite(PIN_LASER3, !state);
}