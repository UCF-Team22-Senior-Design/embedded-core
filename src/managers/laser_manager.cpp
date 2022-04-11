#include "laser_manager.h"

// Static variables of our class must be initialized in the .cpp file - so, we
// initialize our stuff here, even if it has an empty/default constructor.
//
// C++ is stupid sometimes.
bool LaserManager::hasBeenInitialized = false;
Task LaserManager::taskLaserDimmer(TASK_MILLISECOND * PULSE_LENGTH, TASK_ONCE, &LaserManager::dimLaserCallback);

LaserMode LaserManager::laserMode = LaserMode::PULSE_ON_TRIGGER;
bool LaserManager::enabled = false;

/**
 * @brief A function to run once at the start of execution, which establishes
 *        the necessary callbacks for our laser manager. A static
 *        variable keeps track of whether it has been initalized or not, so you
 *        cannot call this twice.
 */
void LaserManager::initialize(Scheduler *userScheduler)
{
    // Check to see if we've been initialized already.
    if (hasBeenInitialized)
        return;

    // Register the laser pins as output
    pinMode(PIN_LASER1, OUTPUT);
    pinMode(PIN_LASER2, OUTPUT);
    pinMode(PIN_LASER3, OUTPUT);

    // Set the laser's state to off
    setLaserState(false);

    // Register a callback for the input manager, listening to the trigger
    InputManager::registerInputCallback(LaserManager::triggerCallback, InputSource::BUTTON_TRIGGER);

    // Register our task to the scheduler
    (*userScheduler).addTask(taskLaserDimmer);

    // Initialization complete
    hasBeenInitialized = true;
}

void LaserManager::triggerCallback(InputSource source, bool state)
{
    if(!enabled && !state)
    {
        // Play click audio
        AudioManager::playAudio("/audio/click.wav");
        return;
    };

    switch (laserMode)
    {
    // If the laser is set to follow the trigger's behavior, then match the inverse of the current state
    // and play the audio when the trigger is pulled.
    case FOLLOWS_TRIGGER:
        setLaserState(!state);

        // If the trigger has been pulled (goes low), fire the laser sound effect
        if (state == false)
        {
            AudioManager::playAudio("/audio/shoot.wav");
        }
        return;
    // If the laser is set to ignore the trigger's behavior, do nothing
    case OFF:
    case ON:
        return;
    // If the laser is set to pulse on trigger, turn on the laser (if appropriate) and enableDelayed the
    // dimmer task
    case PULSE_ON_TRIGGER:
        // Only act on the trigger pull
        if (state)
            break;

        // Set the laser's mode high
        setLaserState(true);

        // EnableDelayed our dimmer task
        taskLaserDimmer.restartDelayed(TASK_MILLISECOND * PULSE_LENGTH);

        // Play audio
        AudioManager::playAudio("/audio/shoot.wav");
    default:
        return;
    }
}

void LaserManager::setLaserMode(LaserMode newLaserMode)
{
    // Disable the dimmer task
    taskLaserDimmer.disable();

    // Reset lasers
    setLaserState(false);

    // Store the new mode
    laserMode = newLaserMode;

    // If the laser is disabled, we don't have to visually change modes
    if(!enabled) return;

    // Change to the new laser mode
    switch (newLaserMode)
    {
    case OFF:
        // Lasers are already off
        break;
    case ON:
        // Turn on the lasers
        setLaserState(true);
        break;
    case PULSE_ON_TRIGGER:
        // Don't have to do anything - behavior happens on trigger
        break;
    case FOLLOWS_TRIGGER:
        // Set the laser state to the current trigger state
        setLaserState(!InputManager::getInputState(InputSource::BUTTON_TRIGGER));
        break;
    }
}

void LaserManager::dimLaserCallback()
{
    // Set our laser's state to low
    setLaserState(false);
}

void LaserManager::setLaserState(bool on)
{
    digitalWrite(PIN_LASER1, on);
    digitalWrite(PIN_LASER2, on);
    digitalWrite(PIN_LASER3, on);
}

void LaserManager::setLaserEnable(bool newEnable)
{
    // Store the new state
    enabled = newEnable;
    
    if(!enabled)
    {
        // We're to disable the laser. Turn off whatever we currently have on, disable the dimmer, and 
        // break;
        setLaserState(false);
        taskLaserDimmer.disable();
        return;
    }

    // We're to enable the laser. Set it to match whatever mode is desired.
    switch (laserMode)
    {
    case OFF:
        // Lasers are already off
        break;
    case ON:
        // Turn on the lasers
        setLaserState(true);
        break;
    case PULSE_ON_TRIGGER:
        // Don't have to do anything - behavior happens on trigger
        break;
    case FOLLOWS_TRIGGER:
        // Set the laser state to the current trigger state
        setLaserState(!InputManager::getInputState(InputSource::BUTTON_TRIGGER));
        break;
    }
}