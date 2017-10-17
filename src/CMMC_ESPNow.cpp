#include "CMMC_ESPNow.h"

void CMMC_ESPNow::init(int mode) {
	if (mode == NOW_MODE_SLAVE) {
		WiFi.disconnect(0);
		WiFi.mode(WIFI_STA);
		delay(100);
		Serial.println("====================");
		Serial.println("   MODE = ESPNOW    ");
		Serial.println("====================");
		WiFi.disconnect();
		Serial.println("Initializing ESPNOW...");
		Serial.println("Initializing... SLAVE");
		esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
	}
	else {
		WiFi.disconnect();
		WiFi.mode(WIFI_STA);
		delay(50);
	}

	if (esp_now_init() == 0) {
		Serial.println("init");
	} else {
		Serial.println("init failed");
		ESP.restart();
		return;
	}

	if (mode == NOW_MODE_CONTROLLER) {
		esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
	} else {
		esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
	}

	esp_now_register_send_cb([](uint8_t* macaddr, uint8_t status) {
		Serial.printf("sent status => [%lu]\r\n", status);
		// utils.printMacAddress(macaddr);
	});

	esp_now_register_recv_cb([](uint8_t *macaddr, uint8_t *data, uint8_t len) {
		//      if (digitalRead(BUTTON_PIN) == HIGH) {
		// PACKET_T pkt;
		// memcpy(&pkt, data, sizeof(pkt));
		// memcpy(&pkt.from, macaddr, 48);
		// SENSOR_T sensorData = pkt.data;
		Serial.write(data, len);
		// digitalWrite(LED, ledState);
		// ledState = !ledState;
	});
};
