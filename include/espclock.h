#ifndef __esp_clock_h__
#define __esp_clock_h__

#if !defined(ESP8266)
  #error This code is designed to run on ESP8266 and ESP8266-based boards! Please check your Tools->Board setting.
#endif

#include <time.h>
#include <espstdlib/espio.h>

class EspClock
{
  public:
    explicit EspClock();

    tm getCurrentTime();
    bool isRunning();
    bool startClock(float_t timezone_offset_hours, float_t daylight_offset_hours); 
    bool updateLocalTime(tm& localTime);

  private:
    bool _isRunning = false;
    time_t _previous_time_sec = 0;
};

#endif // __esp_clock_h__
