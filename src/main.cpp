/*****************************************************************************
 
    Daylight savings examples:
        https://werner.rothschopf.net/202011_arduino_esp8266_ntp_en.htm

******************************************************************************/


#include <Arduino.h>

#include "displaydriver.h"
#include "greeting.h"
#include "espclock.h"
#include "espwifi.h"
#include "espstdlib/espio.h"
#include "espstdlib/espinfo.h"

#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <time.h>

/* Configuration of NTP */
#define NTP_SERVER1 "pool.ntp.org"  
#define NTP_SERVER2 "time.nist.gov"  
#define NTP_SERVER3 "time.windows.com"  
/*
    South Australia Daylight Saving: 
    Sun, 1 Oct 2023 – Sun, 7 Apr 2024
    Sun, 6 Oct 2024 – Sun, 6 Apr 2025
    Clock moves forward 1 hour during daylight saving.
    See:  https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
          https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
*/
#define SOUTH_AUSTRALIA_TZ "ACST-9:30ACDT,M10.1.0/02,M4.1.0/03"   

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
EspClock espClock;

char* pcHostDomain = 0;                        // Negotiated host domain
bool bHostDomainConfirmed = false;             // Flags the confirmation of the host domain
MDNSResponder::hMDNSService hMDNSService = 0;  // The handle of the clock service in the MDNS responder

#define SERVICE_PORT 80  // HTTP port

// HTTP server at port 'SERVICE_PORT' will respond to HTTP requests
ESP8266WebServer server(SERVICE_PORT);

/*
   getTimeString
*/
const char* getTimeString(void) {


  static char acTimeString[32];
  time_t now = time(nullptr);
  ctime_r(&now, acTimeString);
  size_t stLength;
  while (((stLength = strlen(acTimeString))) && ('\n' == acTimeString[stLength - 1])) {
    acTimeString[stLength - 1] = 0;  // Remove trailing line break...
  }
  return acTimeString;
}

//#define TIMEZONE_OFFSET 1        // CET
//#define DST_OFFSET 1             // CEST
#define UPDATE_CYCLE (1 * 1000)  // every second


/*
   setClock

   Set time via NTP
*/
/*
void setClock(void) 
{
    //configTzTime("AU", "pool.ntp.org", "time.nist.gov", "time.windows.com");
    //configTime((TIMEZONE_OFFSET * 3600), (DST_OFFSET * 3600), "pool.ntp.org", "time.nist.gov", "time.windows.com");

    
    Serial.print("\nWaiting for NTP time sync: ");
    time_t now = time(nullptr);   // Secs since 01.01.1970 (when uninitialized starts with (8 * 3600 = 28800)
    while (now < 8 * 3600 * 2) {  // Wait for realistic value
      delay(100);
      Serial.print(".");
      now = time(nullptr);
    }
    Serial.println("");
    Serial.printf("Current time: %s\n", getTimeString());
}
*/
/*
   setStationHostname
*/
bool setStationHostname(const char* p_pcHostname) {

  if (p_pcHostname) {
    WiFi.hostname(p_pcHostname);
    Serial.printf("setDeviceHostname: Station hostname is set to '%s'\n", p_pcHostname);
  }
  return true;
}

/*
   MDNSDynamicServiceTxtCallback

   Add a dynamic MDNS TXT item 'ct' to the clock service.
   The callback function is called every time, the TXT items for the clock service
   are needed.
   This can be triggered by calling MDNS.announce().

*/
void MDNSDynamicServiceTxtCallback(const MDNSResponder::hMDNSService p_hService) 
{
    Serial.println("MDNSDynamicServiceTxtCallback");
    if (hMDNSService == p_hService) 
    {
        Serial.printf("Updating curtime TXT item to: %s\n", getTimeString());
        MDNS.addDynamicServiceTxt(p_hService, "curtime", getTimeString());
    }
}

/*
   MDNSProbeResultCallback

   Probe result callback for the host domain.
   If the domain is free, the host domain is set and the clock service is
   added.
   If the domain is already used, a new name is created and the probing is
   restarted via p_pMDNSResponder->setHostname().

*/
void hostProbeResult(String p_pcDomainName, bool p_bProbeResult) {

  Serial.println("MDNSProbeResultCallback");
  Serial.printf("MDNSProbeResultCallback: Host domain '%s.local' is %s\n", p_pcDomainName.c_str(), (p_bProbeResult ? "free" : "already USED!"));
  if (true == p_bProbeResult) {
    // Set station hostname
    setStationHostname(pcHostDomain);

    if (!bHostDomainConfirmed) {
      // Hostname free -> setup clock service
      bHostDomainConfirmed = true;

      if (!hMDNSService) {
        // Add a 'clock.tcp' service to port 'SERVICE_PORT', using the host domain as instance domain
        hMDNSService = MDNS.addService(0, "espclk", "tcp", SERVICE_PORT);
        if (hMDNSService) {
          // Add a simple static MDNS service TXT item
          MDNS.addServiceTxt(hMDNSService, "port#", SERVICE_PORT);
          // Set the callback function for dynamic service TXTs
          MDNS.setDynamicServiceTxtCallback(MDNSDynamicServiceTxtCallback);
        }
      }
    }
  } else {
    // Change hostname, use '-' as divider between base name and index
    if (MDNSResponder::indexDomain(pcHostDomain, "-", 0)) {
      MDNS.setHostname(pcHostDomain);
    } else {
      Serial.println("MDNSProbeResultCallback: FAILED to update hostname!");
    }
  }
}

void handleHTTPRequest() {
  Serial.println("");
  Serial.println("HTTP Request");

  // Get current time
  time_t now = time(nullptr);
  
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);

  String s;

  s = "<!DOCTYPE HTML>\r\n<html>Hello from ";
  s += WiFi.hostname() + " at " + WiFi.localIP().toString();
  // Simple addition of the current time
  s += "\r\nCurrent time is: ";
  s += getTimeString();
  // done :-)
  s += "</html>\r\n\r\n";
  Serial.println("Sending 200");
  server.send(200, "text/html", s);
}

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

    // Here is the IMPORTANT ONE LINER needed in your sketch!
    //configTime(SOUTH_AUSTRALIA_TZ, NTP_SERVER1, NTP_SERVER2, NTP_SERVER3); 
    
    ledclock::connectToWiFi();

    /*   
    MDNS.setHostProbeResultCallback(hostProbeResult);
    
    // Init the (currently empty) host domain string with 'esp8266'
    if ((!MDNSResponder::indexDomain(pcHostDomain, 0, "esp8266")) || (!MDNS.begin(pcHostDomain))) 
    {
        Serial.println("Error setting up MDNS responder!");
        while (1) {  // STOP
          delay(1000);
        }
    }
    Serial.println("MDNS responder started");

    // Setup HTTP server
    server.on("/", handleHTTPRequest);
    server.begin();
    Serial.println("HTTP server started");


    std::cout << std::endl;
    std::cout << "Setup Complete" << std::endl;
    std::cout << ".............................................................................\n";
    ***/
}

void showTime() 
{
    time_t now;                         // this are the seconds since Epoch (1970) - UTC
    tm tm; 

    time(&now);                       // read the current time
    localtime_r(&now, &tm);           // update the structure tm with the current time
    Serial.print("year: ");
    Serial.print(tm.tm_year + 1900);  // years since 1900
    Serial.print("   month: ");
    Serial.print(tm.tm_mon + 1);      // January = 0 (!)
    Serial.print("   day: ");
    Serial.print(tm.tm_mday);         // day of month
    Serial.print("   hour: ");
    Serial.print(tm.tm_hour);         // hours since midnight  0-23
    Serial.print("   min: ");
    Serial.print(tm.tm_min);          // minutes after the hour  0-59
    Serial.print("   sec: ");
    Serial.print(tm.tm_sec);          // seconds after the minute  0-61*
    Serial.print("   wday: ");
    Serial.print(tm.tm_wday);         // days since Sunday 0-6
    if (tm.tm_isdst == 1)             // Daylight Saving Time flag
        Serial.print("   DST");
    else
        Serial.print("   standard");
    Serial.println();
}
  // -----------------------------------------------------------------------------------------

void loop()
{
    if (!espClock.isRunning())
    {
        espClock.startClock(SOUTH_AUSTRALIA_TZ, NTP_SERVER1);
    } 
    else
    {
        tm localTime;
        if (espClock.updateLocalTime(localTime))
        {
            displayDriver.displayTime(localTime);
            showTime();
        }
    }

    

  /****
  // Check if a request has come in
  server.handleClient();
  // Allow MDNS processing
  MDNS.update();

  static esp8266::polledTimeout::periodicMs timeout(UPDATE_CYCLE);
  if (timeout.expired()) {

    if (hMDNSService) {
      // Just trigger a new MDNS announcement, this will lead to a call to
      // 'MDNSDynamicServiceTxtCallback', which will update the time TXT item
      MDNS.announce();
    }
  }

*/
}

/*
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
*/


