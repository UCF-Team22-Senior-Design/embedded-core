#pragma once

#include <Arduino.h>
#include <painlessMesh.h>
#include <unordered_map>

#include "../models/pinouts.h"

#define DEBOUNCE_DELAY 50

enum InputSource 
{
    /// The left input button on the controller, used for navigating menus.
    BUTTON_LEFT,
    /// The right input button on the controller, used for navigating menus.
    BUTTON_RIGHT,
    /// The primary trigger button on the controller, used for navigating menus
    /// and for gameplay.
    BUTTON_TRIGGER,
    /// The Pairing button on the target, used for setting the device into a 
    /// "Pairing" state
    BUTTON_PAIR,
    /// The phototransistor on the target, used for detecting laser hits on the
    /// target.
    PHOTOTRANSISTOR,
    EXAMPLE,
    /// A dummy source used for unfiltered callbacks. Does not correspond to a
    /// physical input on the device.
    ALL
};

typedef void(*InputReceivedCallback)(InputSource source, bool state);

class InputManager
{
private:
    static std::unordered_map<InputSource, std::vector<InputReceivedCallback>, std::hash<int>> callbacks;
    static std::unordered_map<InputSource, bool, std::hash<int>> inputStates;

    static bool hasBeenInitialized;
    static void inputHappened(InputSource source, bool state);

    // ExampleButton Functions
    static Task exampleButtonResponseTask;
    static void exampleButtonCallback();
    static void IRAM_ATTR exampleButtonInterrupt();
public:
    InputManager();

    static void initialize(Scheduler *scheduler);
    static bool getInputState(InputSource source);
    static void registerInputCallback(InputReceivedCallback callback, InputSource source = InputSource::ALL);
    static void deregisterInputCallback(InputReceivedCallback callback, InputSource source = InputSource::ALL);
};
