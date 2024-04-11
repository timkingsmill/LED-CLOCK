#pragma once

#include <espstdlib/espio.h>

#if !defined(ESP8266)
  #error This code is designed to run on ESP8266 and ESP8266-based boards! Please check your Tools->Board setting.
#endif

#define banner(out) (out \
    <<    std::endl \
    <<    "............................................................................   \n" \
    <<    "  _______   _                          ___     __                    _         \n" \
    <<    " (__ _ __) (_)              ____     _(___)_  (__)                  (_)  _     \n" \
    <<    "    (_)     _    __   __   (____)   (_)   (_)  (_)    ___      ___  (_)_(_)    \n" \
    <<    "    (_)    (_)  (__)_(__)  (_)__    (_)    _   (_)   (___)   _(___) (___)      \n" \
    <<    "    (_)    (_) (_) (_) (_)  _(__)   (_)___(_)  (_)  (_)_(_) (_)___  (_) (_)    \n" \
    <<    "    (_)    (_) (_) (_) (_) (____)     (___)   (___)  (___)   (____) (_)  (_)   \n" \
    <<    ".............................................................................  \n" \
)  

void printGreeting(ostream& output)
{
    banner(output) << std::endl;
};
