#include "espclock.h"

EspClock::EspClock()
{
}

tm EspClock::getCurrentTime()
{
    tm localTime;
    ::getLocalTime(&localTime, 0);
    return localTime;
}

