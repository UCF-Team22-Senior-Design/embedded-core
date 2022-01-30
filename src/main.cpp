#include <Arduino.h>

/* -- Dependencies for painlessMesh -- */
#include <WiFi.h>
#include <SPIFFS.h>
#include <Update.h>
#include <TaskScheduler.h>

/* -- Project Includes -- */
#include "managers/state_manager.h"
#include "managers/input_manager.h"

StateManager stateManager;
InputManager inputManager;

Scheduler userScheduler;

void setup() {
  // Initialize Serial Communication
  Serial.begin(115200);
  Serial.println("Hello, world!");

  // -- Initialize hardware managers-- //
  inputManager.initialize(&userScheduler);
}

void loop() {
  // put your main code here, to run repeatedly:
  userScheduler.execute();
}