#include "module.h"

/// The functional core of a module - this controls everything. Callbacks
/// are assigned to this to actually implement functionality.
Task Module::moduleTask;

/**
 * @brief The basic initialization of a module - must be done to ensure the
 *        proper binding of the task to its callbacks and scheduler. Should only
 *        be done once.
 * 
 * @param userScheduler 
 */
void Module::initialize(Scheduler *userScheduler)
{
    static bool hasBeenInitialized = false;
    if(hasBeenInitialized) return;
    hasBeenInitialized = true;

    // Configure our task
    moduleTask.set(TASK_MILLISECOND * 16, TASK_FOREVER, &onUpdate, &onWake, &onSleep);

    // Bind it to our scheduler
    (*userScheduler).addTask(moduleTask);
}

/**
 * @brief A helper to return a pointer to this module's task.
 * 
 * @return Task* 
 */
Task* Module::getTask()
{
    return &moduleTask;
}