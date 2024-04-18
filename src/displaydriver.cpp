#include "displaydriver.h"
#include "espstdlib/espio.h"

namespace ledclock
{
    enum class Segment : byte
    {
        A =  0b00001000,
        B =  0b00000100,
        C =  0b00000010,
        D =  0b00100000,
        E =  0b10000000,
        F =  0b01000000,
        G =  0b00010000,
        DP = 0b00000001
    };

    inline Segment operator | (Segment lhs, Segment rhs)
    {
        using T = std::underlying_type_t <Segment>;
        return static_cast<Segment>(static_cast<T>(lhs) | static_cast<T>(rhs));
    }



    // Holders for characters or bitmap information to pass to shifting function

    const std::vector<byte> characters
    {
        static_cast<byte>(Segment::A | Segment::B | Segment::C | Segment::D | Segment::E  | Segment::F),
        static_cast<byte>(Segment::B | Segment::C),    
        static_cast<byte>(Segment::A | Segment::B | Segment::D | Segment::E | Segment::G),  
        static_cast<byte>(Segment::A | Segment::B | Segment::C | Segment::D | Segment::G),    
        static_cast<byte>(Segment::B | Segment::C | Segment::F | Segment::G),
        static_cast<byte>(Segment::A | Segment::C | Segment::D | Segment::F | Segment::G),     
        static_cast<byte>(Segment::A | Segment::C | Segment::D | Segment::E | Segment::F | Segment::G),     
        static_cast<byte>(Segment::A | Segment::B | Segment::C),    
        static_cast<byte>(Segment::A | Segment::B | Segment::C),    
        static_cast<byte>(Segment::A | Segment::B | Segment::C | Segment::D | Segment::E | Segment::F | Segment::G),
        static_cast<byte>(Segment::A | Segment::B | Segment::C | Segment::D | Segment::F | Segment::G),
        static_cast<byte>(Segment::A | Segment::B | Segment::C | Segment::E | Segment::F | Segment::G),
        static_cast<byte>(Segment::C | Segment::D | Segment::E | Segment::F | Segment::G),
        static_cast<byte>(Segment::A | Segment::D | Segment::E | Segment::F),
        static_cast<byte>(Segment::B | Segment::C | Segment::D | Segment::E | Segment::G),
        static_cast<byte>(Segment::A | Segment::D | Segment::E | Segment::F | Segment::G),
        static_cast<byte>(Segment::A | Segment::E | Segment::F | Segment::G)
    };

    // --------------------------------------------------------------------------------

    DisplayDriver::DisplayDriver(uint8_t dataPin, uint8_t latchPin, uint8_t clockPin)
        :   _dataPin(dataPin), 
            _latchPin(latchPin), 
            _clockPin(clockPin) 
    {
        pinMode(_dataPin, OUTPUT);
        pinMode(_latchPin, OUTPUT);
        pinMode(_clockPin, OUTPUT);

        //Scheduler.start(&_printTask);
    }

    // ----------------------------------------------------------

    void DisplayDriver::clear(void)
    {
        int data = LOW;
        digitalWrite(_dataPin, data);
        for (int segment = 0; segment < 10; segment++)
        {
            _pulsePin(_clockPin);
            _pulsePin(_latchPin);
        }
    }

    // ----------------------------------------------------------

    void DisplayDriver::displayTime(const tm& time)
    {
        std::cout << "Clock Has Updated" << std::endl;
        using T = std::underlying_type_t<Segment>;
        
        byte buffer[6] {0, 0, 0, 0 ,0, 0};

        int seconds = time.tm_sec;
        buffer[0] = characters[seconds % 10];
        buffer[1] = characters[seconds / 10];

        int minites = time.tm_min;
        buffer[2] = characters[minites % 10];
        buffer[3] = characters[minites / 10];

        buffer[2] = buffer[2] | static_cast<T>(Segment::DP);
        buffer[3] = buffer[3] | static_cast<T>(Segment::DP);

        int hours = (time.tm_hour % 12);
        if (hours == 0)
            hours = 12;

        buffer[4] = characters[hours % 10];
        if (hours > 9)
        {
            buffer[5] = characters[1]; 
        }

        // Set the AM / PM led.
        int hours24 = time.tm_hour;
        if ((hours24 == 24) || (hours24 < 12))
        {
            // AM
            buffer[5] = buffer[5] | static_cast<T>(Segment::DP);
        }
        else 
        {
            // PM
            buffer[4] = buffer[4] | static_cast<T>(Segment::DP);
        }  

        for (int digit = 6; digit >= 0; digit--)
        {
            _shiftBitmapOut(buffer[digit]);
        }
        _pulsePin(_latchPin);
    }

    // ----------------------------------------------------------

    void DisplayDriver::_shiftBitmapOut(byte bitmap)
    {
        for (int i = 7 ; i >= 0; i--)  
        {
            int bit = (bitmap & (1 << i)) ? LOW : HIGH;
            digitalWrite(_dataPin, bit);
            _pulsePin(_clockPin);
        }
    }

    // ----------------------------------------------------------

    void DisplayDriver::setAllSegments(int state)
    {
        //_printTask.En
        digitalWrite(_dataPin, state);
        for (int segment = 0; segment < 6 * 8; segment++)
        {
            _pulsePin(_clockPin);
        }
        _pulsePin(_latchPin);
    }

    // ----------------------------------------------------------

    void DisplayDriver::shiftSegmentOut(void)
    {
    // _printTask.enable();

        digitalWrite(_dataPin, 0);
        for (int segment = 0; segment < 6 * 8; segment++)
        {
            _pulsePin(_clockPin);
            _pulsePin(_latchPin);
            digitalWrite(_dataPin, 255);
            delay(50);
        }
        //_printTask.disable();
    }

    void DisplayDriver::_pulsePin(uint8_t pin)
    {
        digitalWrite(pin, LOW);
        digitalWrite(pin, HIGH);
        digitalWrite(pin, LOW);
    }
}