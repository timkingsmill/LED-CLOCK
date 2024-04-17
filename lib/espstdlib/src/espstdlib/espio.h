#ifndef __esp_io_h__
#define __esp_io_h__

#include "iostream/ArduinoStream.h"

namespace std
{

  static ArduinoOutStream cerr(Serial);
  static ArduinoOutStream cout(Serial);

  inline ostream& endl(ostream& os) 
  {
    os.put('\n');
  #if ENDL_CALLS_FLUSH
    os.flush();
  #endif  // ENDL_CALLS_FLUSH
    return os;
  }
}

#endif // __esp_io_h__
