#ifndef __esp_time_h__
#define __esp_time_h__

#if !defined(ESP8266)
  #error This code is designed to run on ESP8266 and ESP8266-based boards! Please check your Tools->Board setting.
#endif

//#include <iostream.h>
#include <iostream/ArduinoStream.h>
#include <time.h>

ostream& operator << (ostream& os, const tm& dt)
{
    os  << "year: "     << dt.tm_year + 1900 
        << "   month: " << dt.tm_mon + 1 
        << "   day: "   << dt.tm_mday
        << "   hour: "  << dt.tm_hour
        << "   min: "   << dt.tm_min
        << "   sec: "   << dt.tm_sec
        << "   wday: "  << dt.tm_wday;
        if (dt.tm_isdst == 1) 
            os << "   DST";
        else
            os << "   standard";
    return os;
}

#endif