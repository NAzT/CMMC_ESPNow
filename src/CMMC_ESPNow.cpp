#include "CMMC_ESPNow.h"

void CMMC_ESPNow::init(int mode) {
  WiFi.disconnect();
  if (mode == NOW_MODE_SLAVE) {
    WiFi.mode(WIFI_STA);
  }
  else {
    WiFi.mode(WIFI_STA);
  }

  if (esp_now_init() == 0) {
		USER_DEBUG_PRINTF("espnow init ok");
  } else {
		USER_DEBUG_PRINTF("espnow init failed");
    ESP.restart();
    return;
  }

  if (mode == NOW_MODE_CONTROLLER) {
    esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  } else {
    esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  }

  esp_now_register_send_cb(this->_system_on_message_sent);
  esp_now_register_recv_cb(this->_system_on_message_recv);
};
