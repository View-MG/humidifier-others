#pragma once
#include <Arduino.h>
#include "constant.h"

class SteamController {
private:
    int _pin;
    bool _currentState;

public:
    SteamController() : _pin(PIN_STEAM), _currentState(false) {}

    void begin() {
        pinMode(_pin, OUTPUT);
        digitalWrite(_pin, LOW);
    }

    // รับคำสั่งเปิด/ปิด (พร้อมเช็คความปลอดภัยขั้นสุดท้าย)
    void setState(bool state, bool isSafeToRun) {
        bool targetState = isSafeToRun ? state : false;

        _currentState = targetState;   // อัปเดต state ทุกครั้ง
        digitalWrite(_pin, _currentState ? HIGH : LOW);

        Serial.printf("[Steam] State: %s  (PIN=%d)\n",
                    _currentState?"ON":"OFF",
                    _currentState?HIGH:LOW);
    }

    bool getState() {
        return _currentState;
    }
};