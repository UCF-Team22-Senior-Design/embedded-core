#include <Arduino.h>

#define PLATFORM_CONTROLLER
#ifndef PLATFORM_CONTROLLER
  #define PLATFORM_TARGET
#endif

/* -- Project Includes -- */
#include "managers/network_manager.h"
#include "managers/state_manager.h"
#include "managers/input_manager.h"
#include "managers/audio_manager.h"
#include "managers/pwm_manager.h"

#include "modules/modules.h"

// Controller-specific managers
#ifdef PLATFORM_CONTROLLER
  #include "managers/laser_manager.h"
#endif

// Target-specific managers
#ifdef PLATFORM_TARGET
  #include "managers/lighting_manager.h"
#endif

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
  
  // Platform-specific managers
#ifdef PLATFORM_CONTROLLER
  LaserManager::initialize();
#endif

#ifdef PLATFORM_TARGET
  LightingManager::initialize(&userScheduler);
#endif

/*
  // Register the gameplay modules to the state manager
  StateManager::registerStateTask(SystemState::Ready,   ReadyModule::getTask());
  StateManager::registerStateTask(SystemState::Pair,    PairModule::getTask());
  StateManager::registerStateTask(SystemState::Play,    PlayModule::getTask());
#ifdef PLATFORM_CONTROLLER
  StateManager::registerStateTask(SystemState::Results, ResultsModule::getTask());
#endif
*/

  // Change our state to ready
  StateManager::setSystemState(SystemState::Ready);
}

void loop() {
  NetworkManager::update();
}