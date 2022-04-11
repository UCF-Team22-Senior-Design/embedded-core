#include <Arduino.h>

#define PLATFORM_CONTROLLER

/* -- Project Includes -- */
#include "models/pinouts.h"
#include "managers/managers.h"
#include "modules/modules.h"

Scheduler userScheduler;

void setup() {
  // Initialize Serial Communication
  Serial.begin(115200);
  Serial.println("{Main} Hello, world!");

  // -- Initialize hardware managers-- //

  // Initialize filesystem
  if(!SPIFFS.begin(true))
  {
      Serial.println("{Main} An error occurred while initializing spiffs");
      return;
  }

  // Verify filesystem is properly configured
  if (SPIFFS.begin()) {
      if (SPIFFS.totalBytes() <= 0) {
        Serial.println("{Main} SPIFFS size was set to 0. Remember to download the filesystem image.");
      }
    } else {
      Serial.println("{Main} Failed to start spiffs");
  }

  // Load our data from the config manager.
  ConfigManager::loadData();

  // Shared managers
  PWMManager::initialize();
  InputManager::initialize(&userScheduler);
  NetworkManager::initialize(&userScheduler);
  AudioManager::initialize(&userScheduler);
  
  // Platform-specific managers
  LaserManager::initialize(&userScheduler);
  DisplayManager::initialize();

  // Register the gameplay modules to the state manager
  ReadyModule::initialize(&userScheduler);
  PairingModule::initialize(&userScheduler);
  TargetTimingModule::initialize(&userScheduler);
  StandbyTestModule::initialize(&userScheduler);
  OneShotModule::initialize(&userScheduler);
  TimeTrialModule::initialize(&userScheduler);
  WhackAMoleModule::initialize(&userScheduler);
  HordeModule::initialize(&userScheduler);

  ResultsModule::initialize(&userScheduler);

  StateManager::registerStateTask(SystemState::Ready,        ReadyModule::getTask());
  StateManager::registerStateTask(SystemState::Pair,         PairingModule::getTask());
  StateManager::registerStateTask(SystemState::TargetTiming, TargetTimingModule::getTask());
  StateManager::registerStateTask(SystemState::StandbyTest,  StandbyTestModule::getTask());

  StateManager::registerStateTask(SystemState::GameOneShot,  OneShotModule::getTask());
  StateManager::registerStateTask(SystemState::GameTimeTrial,  TimeTrialModule::getTask());
  StateManager::registerStateTask(SystemState::GameWhackAMole,  WhackAMoleModule::getTask());
  StateManager::registerStateTask(SystemState::GameHorde,      HordeModule::getTask());

  StateManager::registerStateTask(SystemState::Results,      ResultsModule::getTask());

/*  StateManager::registerStateTask(SystemState::Play,    PlayModule::getTask());
#ifdef PLATFORM_CONTROLLER
  StateManager::registerStateTask(SystemState::Results, ResultsModule::getTask());
#endif
*/

  // Change our state to ready
  StateManager::setSystemState(SystemState::Ready);

  Serial.println("{Main} Setup done.");
}

void loop() {
  NetworkManager::update();
}