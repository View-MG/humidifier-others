#pragma once
#include <WiFi.h>
#include <esp_now.h>
#include "constant.h"

typedef void (*CommandCallback)(uint8_t,bool);

class NetworkService {
private:
    SensorPacket pkt;
    static CommandCallback cb;

    static void OnRecv(const uint8_t*, const uint8_t* data, int len){
        if(len == sizeof(CommandPacket)){
            CommandPacket cmd;
            memcpy(&cmd, data, sizeof(cmd));

            if(cb) cb(cmd.deviceType, cmd.active);
        }
    }

    static void OnSent(const uint8_t*, esp_now_send_status_t s){
        Serial.printf("[SEND_STATUS] %s\n", s==ESP_NOW_SEND_SUCCESS ? "OK" : "FAIL");
    }

public:
    NetworkService(){
        pkt.nodeId      = 1;
        pkt.waterRaw    = 0;
        pkt.waterPercent= 0;
        pkt.isTilted    = false;
        pkt.keyPress    = 0;
        pkt.fanStatus   = false;
        pkt.steamStatus = false;
        pkt.temperature = 0;
        pkt.humidity    = 0;
    }

    void begin(CommandCallback _cb){
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

        Serial.println("\n=== SENSOR NODE BOOT ===");
        Serial.print("[Sensor MAC] "); Serial.println(WiFi.macAddress());
        Serial.printf("[WiFi Channel] %d\n", WiFi.channel());

        if(esp_now_init() != ESP_OK){
            Serial.println("[ESP-NOW] Init failed");
            return;
        }

        cb = _cb;
        esp_now_register_recv_cb(OnRecv);
        esp_now_register_send_cb(OnSent);

        esp_now_peer_info_t peer = {};
        memcpy(peer.peer_addr, GATEWAY_MAC, 6);
        peer.channel = 0;
        peer.encrypt = false;
        esp_now_add_peer(&peer);

        Serial.println("[ESP-NOW READY]\n");
    }

    // ✅ ส่งสถานะกลับ Gateway — ลำดับพารามิเตอร์ตรงกับ main.cpp แล้ว
    void send(int raw, int pct, bool tilt, char key, bool fan, bool steam){
        pkt.waterRaw     = raw;
        pkt.waterPercent = pct;
        pkt.isTilted     = tilt;
        pkt.keyPress     = key;

        pkt.fanStatus    = fan;
        pkt.steamStatus  = steam;

        // ตอนนี้ยังไม่มี temp/humid จริง → เซ็ตเป็น 0 ไปก่อน
        pkt.temperature  = 0.0f;
        pkt.humidity     = 0.0f;

        esp_now_send(GATEWAY_MAC, (uint8_t*)&pkt, sizeof(pkt));

        Serial.printf("[Sensor→Gateway] FAN=%s STEAM=%s (raw=%d pct=%d)\n",
                      fan?"ON":"OFF",
                      steam?"ON":"OFF",
                      raw, pct);
    }
};

CommandCallback NetworkService::cb = nullptr;
