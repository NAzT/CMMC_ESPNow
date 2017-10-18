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
        that->_waiting_message_has_arrived = true;
        that->_user_on_message_recv(macaddr, data, len);
      };

      this->_system_on_message_sent = [](uint8_t *macaddr, u8 status) {
        that->_message_sent_status = status;
        that->_user_on_message_sent(macaddr, status);
      };
    }

    ~CMMC_ESPNow() {}

    void init(int mode);
    void enable_retries() { }
    void send(uint8_t *mac, u8* data, int len, void_cb_t cb = NULL, uint32_t wait_time = 500) {
      this->_message_sent_status = -1;
      this->_waiting_message_has_arrived = false;

      uint32_t MAX_RETRIES   = 10;
      uint32_t RETRIES_DELAY = 20;
      int retries = 0;

      esp_now_send(mac, data, len);
      delay(RETRIES_DELAY);

      while(this->_enable_retries && this->_message_sent_status != 0) {
        USER_DEBUG_PRINTF("try to send over espnow...");
        esp_now_send(mac, data, len);
        delay(RETRIES_DELAY);
        if (++retries > MAX_RETRIES) {
          break;
        }
      }

      uint32_t timeout_at_ms = millis() + wait_time;
      while (millis() < timeout_at_ms) {
        USER_DEBUG_PRINTF("Waiting a command message...");
        delay(RETRIES_DELAY);
      }

      if (cb != NULL && this->_waiting_message_has_arrived==false) {
        USER_DEBUG_PRINTF("Timeout...");
        cb();
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

    void enable_retries(bool s) {
      this->_enable_retries = s;
    }
  private:
    char debug_buffer[60];
    cmmc_debug_cb_t _user_debug_cb;
    esp_now_recv_cb_t _system_on_message_recv;
    esp_now_send_cb_t _system_on_message_sent;

    esp_now_recv_cb_t _user_on_message_recv;
    esp_now_send_cb_t _user_on_message_sent;
    uint8_t _message_sent_status;
    bool _enable_retries = false;
    bool _waiting_message_has_arrived;
};

#endif //CMMC_ESPNow_H
