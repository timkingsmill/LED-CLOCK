#include <espstdlib/espio.h>
#include "espclock.h"


EspClock::EspClock()
{
}

void EspClock::startClock(const char* tz, const char* server1, const char* server2, const char* server3)
{
    if (!_isRunning)
    {
        std::cout << "Starting the clock. Waiting for NTP time sync: ";

        configTime(tz, server1, server2, server3); 

        std::cout << std::endl;
        _isRunning = true;
    }
   
}

bool EspClock::updateLocalTime(tm& localTime)
{
    if (!_isRunning)
    {
        std::cerr << "Error: Clock update has failed. The clock has not been started." << endl;
        return false;
    }

    // Seconds since Epoch (1970) - UTC
    time_t current_time_sec = ::time(nullptr);
    if (current_time_sec <= _previous_time_sec) 
    {
        return false;
    }
    _previous_time_sec = current_time_sec;

    if (::getLocalTime(&localTime, 0))
    {
        return true;
    }
    return false;
}
