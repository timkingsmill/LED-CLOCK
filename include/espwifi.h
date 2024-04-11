#ifndef __esp_wifi_h__
#define __esp_wifi_h__

#if !defined(ESP8266)
  #error This code is designed to run on ESP8266 and ESP8266-based boards! Please check your Tools->Board setting.
#endif

namespace ledclock
{
    void connectToWiFi(void);
}

#endif