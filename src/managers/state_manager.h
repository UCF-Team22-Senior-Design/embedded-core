#pragma once

#include <Arduino.h>
#include <painlessMesh.h>
#include <unordered_map>

enum SystemState {
    Initial,
    Ready,
    Pair,
    Play,
    Results,
    TargetTiming,
    GameOneShot,
    GameTimeTrial
};

class StateManager
{
    private:
        static SystemState systemState;

        static std::unordered_map<SystemState, Task*, std::hash<int>> stateTasks;
    public:
        static SystemState getSystemState();
        static void setSystemState(SystemState newState);
        static void registerStateTask(SystemState state, Task* task);
        static void removeStateTask(SystemState state);
};