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

#define USER_DEBUG_PRINTF(fmt, args...) { \
    sprintf(this->debug_buffer, fmt, ## args); \
    _user_debug_cb(this->debug_buffer); \
  }

typedef void (*cmmc_debug_cb_t)(const char* message);
typedef void (*void_cb_t)();

class CMMC_ESPNow
{
  public:
    // constructor
    CMMC_ESPNow() {
      static CMMC_ESPNow* that = this;
      this->_user_debug_cb = [](const char* s) { };
      this->_user_on_message_recv = [](uint8_t *macaddr, uint8_t *data, uint8_t len) {};
      this->_user_on_message_sent = [](uint8_t *macaddr, u8 status) {};

      // set system cb
      this->_system_on_message_recv = [](uint8_t *macaddr, uint8_t *data, uint8_t len) {
        that->_user_on_message_recv(macaddr, data, len);
      };

      this->_system_on_message_sent = [](uint8_t *macaddr, u8 status) {
        that->_user_on_message_sent(macaddr, status);
      };
    }

    ~CMMC_ESPNow() {}

    void init(int mode);
    void send(uint8_t *mac, u8* data, int len, void_cb_t cb = NULL,
              uint32_t wait_time = 200) {
      esp_now_send(mac, data, len);
      uint32_t call_send_at_ms = millis();
      while (true) {
        USER_DEBUG_PRINTF("Waiting a command message...");
        if (millis() > (call_send_at_ms + wait_time)) {
          USER_DEBUG_PRINTF("WAIT A COMMAND... TIME OUT");
          if (cb != NULL) {
            cb();
          }
          return;
        }
        delay(100);
      }
    }

    void on_message_recv(esp_now_recv_cb_t cb) {
      if (cb != NULL) {
        this->_user_on_message_recv = cb;
      }
    }

    void on_message_sent(esp_now_send_cb_t cb) {
      if (cb != NULL) {
        this->_user_on_message_sent = cb;
      }
    }
  private:
    char debug_buffer[60];
    cmmc_debug_cb_t _user_debug_cb;
    esp_now_recv_cb_t _system_on_message_recv;
    esp_now_send_cb_t _system_on_message_sent;

    esp_now_recv_cb_t _user_on_message_recv;
    esp_now_send_cb_t _user_on_message_sent;
};

#endif //CMMC_ESPNow_H
