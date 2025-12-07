#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "constant.h"

typedef void (*CommandCallback)(uint8_t deviceType, bool active);

class NetworkService {
private:
    SensorPacket pkt;
    static CommandCallback _cb;

    static void onRecv(const uint8_t* mac, const uint8_t* data, int len) {
        if (len == sizeof(CommandPacket)) {
            CommandPacket cmd;
            memcpy(&cmd, data, sizeof(cmd));

            if (_cb) {
                _cb(cmd.deviceType, cmd.active);
            }
        }
    }

    static void onSent(const uint8_t* mac, esp_now_send_status_t status) {
        Serial.printf("[SEND_STATUS] %s\n",
                      status == ESP_NOW_SEND_SUCCESS ? "OK" : "FAIL");
    }

public:
    NetworkService() {
        pkt.nodeId        = 1;
        pkt.waterRaw      = 0;
        pkt.waterPercent  = 0;
        pkt.tiltState     = (uint8_t)TILT_NORMAL;
        pkt.keyPress      = 0;
        pkt.controlState  = false;
    }

    void begin(CommandCallback cb) {
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

        Serial.println("=== SENSOR NODE BOOT ===");
        Serial.printf("[WiFi] SSID=%s (STA)\n", WIFI_SSID);

        unsigned long start = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - start < 8000) {
            delay(500);
            Serial.print(".");
        }
        Serial.println();
        Serial.print("[WiFi] MAC="); Serial.println(WiFi.macAddress());
        Serial.printf("[WiFi] Channel=%d\n", WiFi.channel());

        if (esp_now_init() != ESP_OK) {
            Serial.println("[ESP-NOW] Init failed");
            return;
        }

        _cb = cb;
        esp_now_register_recv_cb(onRecv);
        esp_now_register_send_cb(onSent);

        esp_now_peer_info_t peer = {};
        memcpy(peer.peer_addr, GATEWAY_MAC, 6);
        peer.channel = 0;      // ใช้ channel เดียวกับ WiFi
        peer.encrypt = false;

        if (esp_now_add_peer(&peer) == ESP_OK) {
            Serial.println("[ESP-NOW] Ready (peer=Gateway)\n");
        } else {
            Serial.println("[ESP-NOW] Failed to add peer\n");
        }
    }

    void send(int waterRaw, int waterPercent, TiltState tilt, char key, bool ctrl) {
        pkt.waterRaw      = waterRaw;
        pkt.waterPercent  = waterPercent;
        pkt.tiltState     = (uint8_t)tilt;
        pkt.keyPress      = key;
        pkt.controlState  = ctrl;

        uint8_t result = esp_now_send(GATEWAY_MAC, (uint8_t*)&pkt, sizeof(pkt));
        if (result != ESP_OK) {
            Serial.printf("[SEND] ERROR code=%d\n", result);
        }
    }
};

CommandCallback NetworkService::_cb = nullptr;
