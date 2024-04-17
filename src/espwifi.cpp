#include "espwifi.h"
#include "espstdlib/espio.h"
#include <ESP8266WiFi.h>

#ifndef STASSID
#define STASSID "tkWiFi24"
//#define STASSID "MobileTim"
#define STAPSK "FLINTSTONE"
#endif

inline constexpr auto ssid = STASSID;
const char* password = STAPSK;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;

namespace ledclock
{
    void onWifiConnect(const WiFiEventStationModeGotIP& event);

   /// @brief Connect to wifi network.
   /// @param  
   void connectToWiFi(void)
   {
        std::cout << "Connecting to WiFi ..";

        wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);

        WiFi.persistent(false);
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);

        // Wait for connection
        while (WiFi.status() != WL_CONNECTED) 
        {
            std::cout << ".";
            delay(500);
        }
       
        //wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);
   }


   void onWifiConnect(const WiFiEventStationModeGotIP& event) 
    {
        std::cout << std::endl;
        std::cout << "Connected to: " << WiFi.SSID() << std::endl;
        std::cout << "IP address:   " << WiFi.localIP().toString();
        std::cout << std::endl;
        WiFi.printDiag(Serial);
    }

}
