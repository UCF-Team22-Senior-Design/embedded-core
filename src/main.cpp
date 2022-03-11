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
  Serial.println("[Main] Hello, world!");

  // -- Initialize hardware managers-- //

  // Initialize filesystem
  if(!SPIFFS.begin(true))
  {
      Serial.println("[Main] An error occurred while initializing spiffs");
      return;
  }

  // Verify filesystem is properly configured
  if (SPIFFS.begin()) {
      if (SPIFFS.totalBytes() > 0) {
        Serial.println("[Main] - Spiffs size is set correctly!");
      } else {
        Serial.println("[checkFlashConfig] - SPIFFS size was set to 0, please select a SPIFFS size from the \"tools->flash size:\" menu or partition a SPIFFS");
      }
    } else {
      Serial.println("[checkFlashConfig] - Failed to start SPIFFS");
  }

  // Load our data from the config manager.
  ConfigManager::loadData();

  // Shared managers
  PWMManager::initialize();
  InputManager::initialize(&userScheduler);
  NetworkManager::initialize(&userScheduler);
  AudioManager::initialize(&userScheduler);
  
  // Platform-specific managers
  LaserManager::initialize();
  DisplayManager::initialize();

  // Register the gameplay modules to the state manager
  ReadyModule::initialize(&userScheduler);
  PairingModule::initialize(&userScheduler);

  StateManager::registerStateTask(SystemState::Ready,   ReadyModule::getTask());
  StateManager::registerStateTask(SystemState::Pair,    PairingModule::getTask());
/*  StateManager::registerStateTask(SystemState::Play,    PlayModule::getTask());
#ifdef PLATFORM_CONTROLLER
  StateManager::registerStateTask(SystemState::Results, ResultsModule::getTask());
#endif
*/

  // Change our state to ready
  StateManager::setSystemState(SystemState::Ready);

  //AudioManager::playAudio("/audio/owin31.wav");

  Serial.println("Setup done.");
}

void loop() {
  NetworkManager::update();
}