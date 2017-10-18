#include "CMMC_ESPNow.h"

void CMMC_ESPNow::init(int mode) {
  if (mode == NOW_MODE_SLAVE) {
    WiFi.disconnect(0);
    WiFi.mode(WIFI_STA);
    delay(100);
    Serial.println("====================");
    Serial.println("   MODE = ESPNOW    ");
    Serial.println("====================");
    Serial.println("Initializing ESPNOW...");
    Serial.println("Initializing... SLAVE");
  }
  else {
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    delay(50);
  }

  if (esp_now_init() == 0) {
    Serial.println("espnow init ok");
  } else {
    Serial.println("espnow init failed");
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
