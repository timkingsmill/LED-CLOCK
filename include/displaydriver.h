#ifndef __display_driver_h__
#define __display_driver_h__

#include <stdint.h>
#include <Scheduler.h>
#include <Task.h>

namespace ledclock
{

    class DisplayDriver
    {
        public:
            explicit DisplayDriver(uint8_t, uint8_t, uint8_t);

            void clear(void);
            void displayTime(const tm& time);
            //void displayTime(const time_t& time);

            void setAllSegments(int state);
            void shiftSegmentOut(void);

        private:
            void _pulsePin(uint8_t);
            void _shiftBitmapOut(byte bitmap); 

        private:
            uint8_t _dataPin;
            uint8_t _latchPin;
            uint8_t _clockPin;
           // PrintTask _printTask;
    };

}






#endif