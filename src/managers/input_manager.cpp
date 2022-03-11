#include "input_manager.h"

// Static variables of our class must be initialized in the .cpp file - so, we
// initialize our stuff here, even if it has an empty/default constructor.
//
// C++ is stupid sometimes.
bool InputManager::hasBeenInitialized = false;
std::unordered_map<InputSource, std::vector<InputReceivedCallback>, std::hash<int>> InputManager::callbacks;
std::unordered_map<InputSource, bool, std::hash<int>> InputManager::inputStates;

// InputTasks
Task InputManager::inputCheckerTask;

InputManager::InputManager() {}

/**
 * @brief A function to run once at the start of execution, which establishes
 *        the necessary callbacks and tasks for our input manager. A static
 *        variable keeps track of whether it has been initalized or not, so you
 *        cannot call this twice.
 * 
 * @param scheduler The scheduler to attach the hardware tasks to.
 */
void InputManager::initialize(Scheduler *scheduler)
{
    // Check to see if we've been initialized already.
    if(hasBeenInitialized) return;

    inputCheckerTask.setInterval(TASK_MILLISECOND * 8);
    inputCheckerTask.setIterations(TASK_FOREVER);
    inputCheckerTask.setCallback(&InputManager::inputCheckerCallback);
    (*scheduler).addTask(inputCheckerTask);
    inputCheckerTask.enable();

    // Configure our input pins
    pinMode(PIN_EXAMPLE, INPUT);
    pinMode(PIN_BUTTON_LEFT, INPUT);
    pinMode(PIN_BUTTON_RIGHT, INPUT);
    pinMode(PIN_TRIGGER, INPUT);

    // Assign default value
    inputStates[InputSource::EXAMPLE] = digitalRead(PIN_EXAMPLE);
    inputStates[InputSource::BUTTON_LEFT] = digitalRead(PIN_BUTTON_LEFT);
    inputStates[InputSource::BUTTON_RIGHT] = digitalRead(PIN_BUTTON_RIGHT);
    inputStates[InputSource::BUTTON_TRIGGER] = digitalRead(PIN_TRIGGER);

    hasBeenInitialized = true;
}
/**
 * @brief Returns the current state of the given input. If InputSource::ALL is
 *        given as the input, this function will always return false. This is
 *        faster / more efficient than simply attempting a digitalRead on a pin,
 *        as it just reads directly from memory.
 * 
 *        Note: This function does not poll the hardware pins to fetch the
 *        current state of that given source - it recalls the last state of the
 *        input from memory. 
 * 
 * @param source the input from which to fetch the current state
 */
bool InputManager::getInputState(InputSource source)
{
    // Always return false for ALL source
    if(source == InputSource::ALL) return false;

    // Fetch the current state of the given input source, otherwise.
    return inputStates[source];
}

/**
 * @brief Registers the provided callback, to be called when the requested 
 *        inputSource changes state
 * 
 *        If InputSource::ALL is provided as source, then the callback will be
 *        called every time any input changes.
 * 
 * @param callback The function to receieve the call when its associated
 *                 inputsource undergoes a change.
 * @param source The source to associate the callback to.
 */
void InputManager::registerInputCallback(InputReceivedCallback callback, InputSource source)
{
    // Store this callback for the associated input source's callback array
    callbacks[source].push_back(callback);
}

/**
 * @brief De-registers the provided callback associated with the given inputsource.
 *        
 *        If InputSource:ALL is provided as the source, then the callback will
 *        be removed from the ALL list - NOT removed from every possible list.
 * 
 *        Callbacks are removed one at a time - if multiple of the same callbacks
 *        are associated with the same source, only the first one will be removed
 *        each time. Successive calls must be made for each de-registration.
 * 
 * @param callback The function to de-register
 * @param source The specific source the function is to be dissasociated from.
 */
void InputManager::deregisterInputCallback(InputReceivedCallback callback, InputSource source)
{
    // First - find the callback in the provided source
    std::vector<InputReceivedCallback> callbackList = callbacks[source];
    
    // Find the position of the callback in the list
    std::vector<InputReceivedCallback>::iterator it;
    for(it = callbackList.begin(); it != callbackList.end(); it++)
    {
        if((*it) == callback)
            break;
    }

    // If we have found it, erase it. Otherwise, it didn't exist.
    if(it != callbackList.end())
    {
        callbackList.erase(it);
        callbacks[source] = callbackList;
    }
    else
    {
        Serial.printf("<InputManager> Unable to find the callback specified for source %d\n", source);
    }
}

/**
 * @brief A centralized function to handle any incoming input events, storing
 *        the new state of the input and calling on the appropriate callbacks.
 * 
 * @param source The inputsource from which this event originated.
 * @param state The new state for this InputSource
 */
void InputManager::inputHappened(InputSource source, bool state)
{
    // Step 1: Store the input state
    inputStates[source] = state;

    // Step 2: Call the appropriate callbacks
    // Step 2a: Call callbacks under the "ALL" listener
    if(callbacks.count(InputSource::ALL) > 0)
    {
        // If there are callbacks for the ALL source, call all of them.
        for(InputReceivedCallback callback : callbacks[InputSource::ALL])
        {
            callback(source, state);
        }
    }

    // Step 2b: Call callbacks for this input source
    if(callbacks.count(source) > 0)
    {
        // If there are callbacks for this input source, call all of them.
        for(InputReceivedCallback callback : callbacks[source])
        {
            //Serial.printf("[InputManager] Issuing Callback %d\n", callback);
            callback(source, state);
        }
    }
    
}

void InputManager::inputCheckerCallback()
{
    // Just get the state of each of the inputs and throw them into the dictionary
    // If they've changed, issue a callback.
    unsigned long now = millis();
    const unsigned long delay = 100;

    bool newInput = digitalRead(PIN_BUTTON_LEFT);
    static unsigned long lastLeftButtonPress = 0;
    if(newInput != inputStates[InputSource::BUTTON_LEFT] && (now - lastLeftButtonPress) > delay)
    {
        inputStates[InputSource::BUTTON_LEFT] = newInput;
        inputHappened(InputSource::BUTTON_LEFT, newInput);
        lastLeftButtonPress = now;
    }

    newInput = digitalRead(PIN_BUTTON_RIGHT);
    static unsigned long lastRightButtonPress = 0;
    if(newInput != inputStates[InputSource::BUTTON_RIGHT] && (now - lastRightButtonPress) > delay)
    {
        inputStates[InputSource::BUTTON_RIGHT] = newInput;
        inputHappened(InputSource::BUTTON_RIGHT, newInput);
        lastRightButtonPress = now;
    }

    newInput = digitalRead(PIN_TRIGGER);
    static unsigned long lastTriggerButtonPress = 0;
    if(newInput != inputStates[InputSource::BUTTON_TRIGGER] && (now - lastTriggerButtonPress) > delay)
    {
        inputStates[InputSource::BUTTON_TRIGGER] = newInput;
        inputHappened(InputSource::BUTTON_TRIGGER, newInput);
        lastTriggerButtonPress = now;
    }
}
