#pragma once
#include <Arduino.h>
#include <Keypad.h>
#include "constant.h"
#include "gyro.h"

class SensorService {
private:
    Keypad _keypad;
    GyroService _gyro;

public:
    SensorService()
        : _keypad(
            makeKeymap(KEY_MAP),
            (byte*)ROW_PINS,
            (byte*)COL_PINS,
            KEYPAD_ROWS,
            KEYPAD_COLS
        )
    {}

    void begin() {
        pinMode(PIN_WATER_LEVEL, INPUT);
        _gyro.begin();
    }

    // เรียกทุก loop เพื่ออัปเดตค่า Gyro ภายใน
    void update() {
        _gyro.update();
    }

    int getWaterRaw() {
        return analogRead(PIN_WATER_LEVEL);
    }

    int getWaterPercent() {
        int raw = getWaterRaw();

        // แปลงจาก WATER_RAW_DRY (3000) → WATER_RAW_FULL (1000) เป็น 0–100%
        // ระวังเพราะ DRY > FULL → ใช้สูตรเอง ไม่ใช้ map ตรง ๆ
        float pct = 100.0f * ( (float)raw - WATER_RAW_DRY )
                          / ( (float)WATER_RAW_FULL - WATER_RAW_DRY );

        if (pct < 0.0f) pct = 0.0f;
        if (pct > 100.0f) pct = 100.0f;

        return (int)pct;
    }

    TiltState getTiltState() const {
        return _gyro.getTiltState();
    }

    // คืนปุ่มล่าสุดที่กด (ถ้าไม่มี = 0)
    char getKey() {
        return _keypad.getKey();
    }
};
