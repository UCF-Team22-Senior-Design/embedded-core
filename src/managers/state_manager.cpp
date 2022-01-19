#include "state_manager.h"

StateManager::StateManager() {
    system_state = SYSTEM_STATE::INITIALIZATION;
}

SYSTEM_STATE StateManager::getSystemState()
{
    return system_state;
}

void StateManager::setSystemState(SYSTEM_STATE newState)
{
    SYSTEM_STATE oldState = system_state;

    // If we're changing to the same state, ignore it.
    if(oldState == newState) return;
    
    // Update our state value
    system_state = newState;

    // Inform listeners
    for(StateCallbackFunction callback: stateChangeListeners)
    {
        callback(oldState, newState);
    }
}

void StateManager::listenForStateChange(StateCallbackFunction changeListener)
{
    stateChangeListeners.insert(changeListener);
}

void StateManager::removeListener(StateCallbackFunction changeListener)
{
    // See if the set contains this callback
    std::unordered_set<StateCallbackFunction>::const_iterator callbackLocation = stateChangeListeners.find(changeListener);
    
    // If it does not, return
    if(callbackLocation == stateChangeListeners.end())
        return;

    // If it does, remove that listener
    stateChangeListeners.erase(callbackLocation);
}