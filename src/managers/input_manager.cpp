#include "input_manager.h"

// Static variables of our class must be initialized in the .cpp file - so, we
// initialize our stuff here, even if it has an empty/default constructor.
//
// C++ is stupid sometimes.
bool InputManager::hasBeenInitialized = false;
std::unordered_map<InputSource, std::vector<InputReceivedCallback>, std::hash<int>> InputManager::callbacks;
std::unordered_map<InputSource, bool, std::hash<int>> InputManager::inputStates;

// InputTasks
Task InputManager::exampleButtonResponseTask;

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

    // Configure our first button task
    exampleButtonResponseTask.setInterval(TASK_IMMEDIATE);
    exampleButtonResponseTask.setIterations(TASK_ONCE);
    exampleButtonResponseTask.setCallback(&InputManager::exampleButtonCallback);
    (*scheduler).addTask(exampleButtonResponseTask);

    // Assign the interrupt for our first button.
    pinMode(PIN_EXAMPLE, INPUT);
    attachInterrupt(digitalPinToInterrupt(PIN_EXAMPLE), InputManager::exampleButtonInterrupt, CHANGE);

    // Assign default value
    inputStates[InputSource::EXAMPLE] = digitalRead(PIN_EXAMPLE);

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
            callback(source, state);
        }
    }
    
}

/**
 * @brief An example ISR - Interrupt Service Routine - that just enables/
 *        restarts the associated task. This way, we catch every event, but also
 *        don't take up too much time from other tasks by accident.
 */
void InputManager::exampleButtonInterrupt()
{
    // Restart / enable the exampleButtonResponseTask
    InputManager::exampleButtonResponseTask.restart();
}

/**
 * @brief An example task callback. This runs when the CPU has the time to, and
 *        reads the state of the button before letting the manager know that an
 *        input has changed.
 */
void InputManager::exampleButtonCallback()
{
    // Fetch the data for this button
    bool state = (digitalRead(PIN_EXAMPLE) == 1);
    // Call the callback thing
    inputHappened(InputSource::EXAMPLE, state);
}