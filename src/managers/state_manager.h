#include "../models/system_states.h"

#include <Arduino.h>
#include <unordered_map>

class StateManager
{
    private:
        SYSTEM_STATE systemState;

        std::unordered_map<SYSTEM_STATE, Task> stateTasks;
    public:
        StateManager();
        SYSTEM_STATE getSystemState();
        void setSystemState(SYSTEM_STATE newState);
        void registerStateTask(SYSTEM_STATE state, Task task);
        void removeStateTask(SYSTEM_STATE state);
};