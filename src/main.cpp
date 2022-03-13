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
  
  LightingManager::initialize(&userScheduler);

  // Register the gameplay modules to the state manager
  ReadyModule::initialize(&userScheduler);
  PairingModule::initialize(&userScheduler);

  StateManager::registerStateTask(SystemState::Ready,   ReadyModule::getTask());
  StateManager::registerStateTask(SystemState::Pair,    PairingModule::getTask());
/*StateManager::registerStateTask(SystemState::Play,    PlayModule::getTask());
*/

  // Change our state to ready

  Serial.println("{Main} Setup done.");

  AudioManager::playAudio("/audio/owin31.wav");

  StateManager::setSystemState(SystemState::Ready);
}

void loop() {
  NetworkManager::update();
}