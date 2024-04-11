#include <Arduino.h>

#include "displaydriver.h"
#include "greeting.h"
#include "espclock.h"
#include "espwifi.h"
#include "espstdlib/espio.h"
#include "espstdlib/espinfo.h"


// Pin connected to Serial In (pin 14) of 74HC595
// Brown Jumper
const int dataPin = D2;

// Pin connected to SH_CP (pin 11) of 74HC595
// Orange Jumper
const int clockPin = D1;
// Pin connected to ST_CP (pin 12) of 74HC595
// Yellow Jumper
const int latchPin = D0;


ledclock::DisplayDriver displayDriver(dataPin, latchPin, clockPin);
//EspClock espClock;

// ------------------------------------------------------------------------------------------

void setup()
{
    Serial.begin(115200);
    // Wait for USB Serial
    while (!Serial) 
    { 
      yield(); 
    };
  
    pinMode(LED_BUILTIN, OUTPUT);

    printGreeting(std::cout);
    printDeviceInfo(std::cout);

    ledclock::connectToWiFi();

    std::cout << std::endl;
    std::cout << "Setup Complete" << std::endl;
    std::cout << ".............................................................................\n";
}

  // -----------------------------------------------------------------------------------------

void loop() 
{

  displayDriver.setAllSegments(HIGH);


  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(1000);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(1000);                      // wait for a second

  displayDriver.setAllSegments(LOW);

  delay(5000);
  printf("Restarting now.\n");
  fflush(stdout);
  ESP.restart();

}


