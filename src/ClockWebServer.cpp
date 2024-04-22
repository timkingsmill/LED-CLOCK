#include "ClockWebServer.h"
#include "espstdlib/espio.h"
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>

#define SERVER_PORT 80  // HTTP port
// HTTP server at port 'SERVICE_PORT' will respond to HTTP requests
ESP8266WebServer _server(SERVER_PORT);
char* _serverDomainName = nullptr;                        // Negotiated host domain

void handleNotFound();
void hostProbeResult(String, bool);
bool setStationHostname(const char*);


void initWebServer(void)
{
    // Setup MDNS responder
    MDNS.setHostProbeResultCallback(hostProbeResult);
    // Init the (currently empty) host domain string with 'clock'
    if ((!MDNSResponder::indexDomain(_serverDomainName, 0, "clock")) || (!MDNS.begin(_serverDomainName))) 
    {
        while (1) 
        { 
            Serial.println("Error setting up MDNS responder!"); 
            // STOP
            delay(1000);
        }
    }
    std::cout << "MDNS responder started: " << _serverDomainName << std::endl;

    _server.on("/", []() 
    {
        _server.send(200, "text/plain", "this works as well");
    });
    _server.onNotFound(handleNotFound);
    _server.begin();
    std::cout << "HTTP server started" << std::endl;
};

void handleWebServerClients(void)
{
    // Check if a request has come in
    _server.handleClient();
    // Allow MDNS processing
    MDNS.update();
};

/*
   MDNSProbeResultCallback

   Probe result callback for the host domain.
   If the domain is free, the host domain is set and the clock service is
   added.
   If the domain is already used, a new name is created and the probing is
   restarted via p_pMDNSResponder->setHostname().

*/
void hostProbeResult(String clockDomainName, bool p_bProbeResult)
{
    static bool bHostDomainConfirmed = false;
    std::cout << "MDNSProbeResultCallback" << std::endl;
    Serial.printf("MDNSProbeResultCallback: Host domain '%s.local' is %s\n", clockDomainName.c_str(), (p_bProbeResult ? "free" : "already USED!"));
    if (true == p_bProbeResult) 
    {
        // Set station hostname
        setStationHostname(_serverDomainName);
    } 
    else 
    {
        // Change hostname, use '-' as divider between base name and index
        if (MDNSResponder::indexDomain(_serverDomainName, "-", 0)) 
        {
            MDNS.setHostname(_serverDomainName);
        } 
        else 
        {
            Serial.println("MDNSProbeResultCallback: FAILED to update hostname!");
        }
    }
}

/*
   setStationHostname
*/
bool setStationHostname(const char* p_pcHostname)
{
    if (p_pcHostname) 
    {
        WiFi.hostname(p_pcHostname);
        Serial.printf("setDeviceHostname: Station hostname is set to '%s'\n", p_pcHostname);
    }
    return true;
}

void handleNotFound() 
{
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += _server.uri();
    message += "\nMethod: ";
    message += (_server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += _server.args();
    message += "\n";
    for (uint8_t i = 0; i < _server.args(); i++) 
    { 
        message += " " + _server.argName(i) + ": " + _server.arg(i) + "\n"; 
    }
    _server.send(404, "text/plain", message);
}
