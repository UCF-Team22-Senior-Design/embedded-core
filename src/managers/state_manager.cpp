#include "state_manager.h"

StateManager::StateManager() {
    systemState = SYSTEM_STATE::INITIAL;
}

/// @returns The current state of the system
SYSTEM_STATE StateManager::getSystemState()
{
    return systemState;
}

/**
 * @brief Shift the state of the system into the new, provided state, disabling
 * the active task of the previous state and enabling the active task of the new
 * state, if one exists in either case.
 * 
 * @param newState The new state of the system.
 */
void StateManager::setSystemState(SYSTEM_STATE newState)
{
    SYSTEM_STATE oldState = systemState;

    // If we're changing to the same state, ignore it.
    if(oldState == newState) return;
    
    // Update our state value
    systemState = newState;

    // Disable the old state's task
    // First, verify that an old task exists
    if(stateTasks.count(oldState) > 0)
        (*stateTasks[oldState]).disable();

    // Enable the new state's task
    if(stateTasks.count(newState) > 0)
        (*stateTasks[newState]).enable();
}

/**
 * @brief Register a state task for the system, one which will be automatically
 * enabled when the state changes to the registered state, and disabled when the
 * state is left.
 * 
 * @note If the tasks is being registered to the active state, then whatever task
 * is currently assigned to the state is disabled and this task is immediately
 * enabled, before being registered. 
 * 
 * @param state The state in which the task shall be enabled
 * @param task The task which shall be enabled in the selected state.
 */
void StateManager::registerStateTask(SYSTEM_STATE state, Task* task)
{
    // Check to see if the state is the one we're currently in.
    if(state == systemState)
    {
        // We're trying to change the task of the active state. Disable our
        // currently active task, then enable this one.
        if(stateTasks.count(systemState) > 0)
            (*stateTasks[systemState]).disable();
        (*task).enable();
    }

    // Store the task inside of the stateTasks array
    stateTasks[state] = task;
}

/**
 * @brief Remove any task association from the given state.
 * 
 * @note If the state is the currently active state, we first disable the
 * task before removing it.
 * 
 * @param state The state to have its task assocations removed
 */
void StateManager::removeStateTask(SYSTEM_STATE state)
{
    // Check to see if the state is the one we're currently in.
    if(state == systemState)
    {
        // We're trying to remove our currently active task. Disable it.
        if(stateTasks.count(systemState) > 0)
            (*stateTasks[systemState]).disable();
    }

    // Remove the task from the stateTasks array
    stateTasks.erase(state);
}