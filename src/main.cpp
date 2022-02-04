#include <Arduino.h>

/* -- Dependencies for painlessMesh -- */


/* -- Project Includes -- */
#include "managers/network_manager.h"
#include "managers/state_manager.h"
#include "managers/input_manager.h"
#include "managers/laser_manager.h"
#include "managers/audio_manager.h"

#include "models/network_message.h"

NetworkManager networkManager;
StateManager   stateManager;
InputManager   inputManager;
LaserManager   laserManager;

Scheduler userScheduler;

void setup() {
  // Initialize Serial Communication
  Serial.begin(115200);
  Serial.println("Hello, world!");

  // -- Initialize hardware managers-- //
  inputManager.initialize(&userScheduler);
  laserManager.initialize();
  networkManager.initialize(&userScheduler);
  AudioManager::initialize(&userScheduler);
}

void loop() {
  networkManager.update();
}