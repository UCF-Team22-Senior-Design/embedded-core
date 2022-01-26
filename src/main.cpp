#include <Arduino.h>

/* -- Dependencies for painlessMesh -- */
#include <WiFi.h>
#include <SPIFFS.h>
#include <Update.h>

/* -- Project Includes -- */
#include "managers/state_manager.h"

StateManager stateManager;

void setup() {
  // Initialize Serial Communication
  Serial.begin(115200);
  Serial.println("Hello, world!");

  // -- Initialize hardware managers-- //

  // -- Prepare tasks / scheduler -- //
}

void loop() {
  // put your main code here, to run repeatedly:
}