#include <Arduino.h>
#include "constant.h"
#include "sensor/sensor.h"
#include "control/control.h"
#include "network.h"

SensorService  sensors;
ControlService controlService;
NetworkService net;

unsigned long lastHeartbeat = 0;
const unsigned long HEARTBEAT_INTERVAL_MS = 3000;

const char* tiltToString(TiltState t) {
    switch (t) {
        case TILT_NORMAL:  return "NORMAL";
        case TILT_WARNING: return "WARN";
        case TILT_FALL:    return "FALL";
        default:           return "?";
    }
}

// ===== Callback เมื่อได้รับคำสั่งจาก Gateway =====
void onGatewayCommand(uint8_t type, bool active) {
    Serial.printf("[CMD] FROM_GATEWAY type=%u state=%s\n",
                  type,
                  active ? "ON" : "OFF");

    // ตอนนี้ treat ทุกคำสั่งเป็น control เดียวกัน
    controlService.setState(active);

    // ส่ง feedback กลับไปหา Gateway ทันที
    int       raw   = sensors.getWaterRaw();
    int       pct   = sensors.getWaterPercent();
    TiltState tilt  = sensors.getTiltState();
    bool      ctrl  = controlService.getState();

    net.send(raw, pct, tilt, '\0', ctrl);

    Serial.printf("[SYNC] CTRL=%s | W=%d%%(%d) | T=%s\n",
                  ctrl ? "ON" : "OFF",
                  pct, raw,
                  tiltToString(tilt));
}

void setup() {
    Serial.begin(115200);

    sensors.begin();
    controlService.begin();
    net.begin(onGatewayCommand);
}

void loop() {
    // อัปเดต Gyro ภายใน
    sensors.update();

    // ---------- KEY EVENT ----------
    char key = sensors.getKey();
    if (key != 0) {
        int       raw   = sensors.getWaterRaw();
        int       pct   = sensors.getWaterPercent();
        TiltState tilt  = sensors.getTiltState();
        bool      ctrl  = controlService.getState();

        net.send(raw, pct, tilt, key, ctrl);

        Serial.printf("[KEY] '%c' | W=%d%%(%d) | T=%s | CTRL=%s\n",
                      key,
                      pct, raw,
                      tiltToString(tilt),
                      ctrl ? "ON" : "OFF");
    }

    // ---------- HEARTBEAT ทุก 3 วิ ----------
    if (millis() - lastHeartbeat > HEARTBEAT_INTERVAL_MS) {
        lastHeartbeat = millis();

        int       raw   = sensors.getWaterRaw();
        int       pct   = sensors.getWaterPercent();
        TiltState tilt  = sensors.getTiltState();
        bool      ctrl  = controlService.getState();

        net.send(raw, pct, tilt, '\0', ctrl);

        Serial.printf("[HB] W=%d%%(%d) | T=%s | CTRL=%s | KEY=-\n",
                      pct, raw,
                      tiltToString(tilt),
                      ctrl ? "ON" : "OFF");
    }
}
