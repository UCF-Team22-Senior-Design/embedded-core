#include "../models/system_states.h"

#include <Arduino.h>
#include <unordered_set>

typedef void(*StateCallbackFunction)(SYSTEM_STATE oldState, SYSTEM_STATE newState);

class StateManager
{
    private:
        SYSTEM_STATE system_state;

        std::unordered_set<StateCallbackFunction> stateChangeListeners;
    public:
        StateManager();
        SYSTEM_STATE getSystemState();
        void setSystemState(SYSTEM_STATE newState);
        void listenForStateChange(StateCallbackFunction changeListener);
        void removeListener(StateCallbackFunction changeListener);
};