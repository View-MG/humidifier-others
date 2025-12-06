#include <Arduino.h>
#include "network.h"
#include "sensor/sensor.h"
#include "control/fan.h"
#include "control/steam.h"

SensorService sensors;
FanController fan;
SteamController steam;
NetworkService net;

void onGatewayCommand(uint8_t type,bool active){
    Serial.println("\n=== CMD RECEIVED ===");

    if(type==CMD_FAN){
        fan.setState(active);
        Serial.printf("→ Fan now = %s\n", active?"ON":"OFF");
    }
    else if(type==CMD_STEAM){
        steam.setState(active,true);
        Serial.printf("→ Steam now = %s\n", active?"ON":"OFF");
    }

    // 🔥 ส่งสถานะกลับ Gateway ทันทีหลัง Set
    net.send(
        sensors.getWaterRaw(), sensors.getWaterPercent(),
        sensors.getTilt(), '\0',
        fan.getState(), steam.getState()
    );

    Serial.println("[SYNC] Sent Feedback back to Gateway\n");
}

void setup(){
    Serial.begin(115200);
    sensors.begin();
    fan.begin();
    steam.begin();
    net.begin(onGatewayCommand);
}

void loop(){
    static unsigned long last = 0;

    // 🟢 Heartbeat Sync ทุก 3 วินาที
    if(millis()-last > 3000){
        last = millis();

        net.send(
            sensors.getWaterRaw(), sensors.getWaterPercent(),
            sensors.getTilt(), '\0',
            fan.getState(), steam.getState()
        );

        Serial.printf("[HB→Gateway] FAN=%d STEAM=%d\n",
                      fan.getState(), steam.getState());
    }
}
