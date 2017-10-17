#ifndef CMMC_ESPNow_H
#define CMMC_ESPNow_H

#include "ESP8266WiFi.h"
#include <functional>

#ifdef ESP8266
extern "C" {
  #include "user_interface.h"
  #include "espnow.h"
}
#endif

#define NOW_MODE_SLAVE 1
#define NOW_MODE_CONTROLLER 2

class CMMC_ESPNow
{
public:
    // constructure
    CMMC_ESPNow() {}
    ~CMMC_ESPNow() {}
    void init(int mode);
    void send(uint8_t *mac, u8* data, int len) {
      esp_now_send(mac, (u8*) &data, len);
    }
};

#endif //CMMC_ESPNow_H
