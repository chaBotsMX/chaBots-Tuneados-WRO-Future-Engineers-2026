#pragma once

#define DEBUG_ENABLED

#ifdef DEBUG_ENABLED
  #define DEBUG_LOG(x) Serial.print(x)
  #define DEBUG_LOGL(x) Serial.println(x)
#else
  #define DEBUG_LOG(x)
  #define DEBUG_LOGL(x)
#endif