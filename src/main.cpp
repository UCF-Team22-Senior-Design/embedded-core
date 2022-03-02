#include <Arduino.h>

#define PLATFORM_TARGET

/* -- Project Includes -- */
#include "models/pinouts.h"
#include "managers/managers.h"
#include "modules/modules.h"

Scheduler userScheduler;

void setup() {
  // Initialize Serial Communication
  Serial.begin(115200);
  Serial.println("Hello, world!");

  // -- Initialize hardware managers-- //

  // Shared managers
  PWMManager::initialize();
  InputManager::initialize(&userScheduler);
  NetworkManager::initialize(&userScheduler);
  AudioManager::initialize(&userScheduler);
  
  LightingManager::initialize(&userScheduler);


  // Register the gameplay modules to the state manager
  ReadyModule::initialize(&userScheduler);

  StateManager::registerStateTask(SystemState::Ready,   ReadyModule::getTask());
/*  StateManager::registerStateTask(SystemState::Pair,    PairModule::getTask());
  StateManager::registerStateTask(SystemState::Play,    PlayModule::getTask());
*/

  // Change our state to ready
  StateManager::setSystemState(SystemState::Ready);

  Serial.println("Setup done.");
}

void loop() {
  NetworkManager::update();
}