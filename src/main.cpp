#include <Arduino.h>
#include "greeting.h"
#include "espclock.h"
#include <espstdlib/espio.h>

EspClock espClock;

void setup() {
  Serial.begin(115200);
  // Wait for USB Serial
  while (!Serial) {
    yield();
  }

  pinMode(LED_BUILTIN, OUTPUT);

  printGreeting(std::cout);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(1000);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(1000);                      // wait for a second
}

