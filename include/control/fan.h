#pragma once
#include <Arduino.h>
#include "constant.h"

class FanController {
private:
    int _pin;
    bool _currentState;

public:
    FanController() : _pin(PIN_FAN), _currentState(false) {}

    void begin() {
        pinMode(_pin, OUTPUT);
        digitalWrite(_pin, LOW); // ค่าเริ่มต้นปิด
    }

    // ============ Updated Version ==============
    void setState(bool state) {
        _currentState = state; 

        // ------------------------------------------
        // หาก Relay = Active HIGH → ใช้แบบนี้
        digitalWrite(_pin, _currentState ? HIGH : LOW);

        // หาก Relay = Active LOW (รีเลย์จีนส่วนใหญ่)
        // digitalWrite(_pin, _currentState ? LOW : HIGH);
        // ------------------------------------------

        Serial.printf("[Fan] Set -> %s (PIN=%d)\n",
                      _currentState ? "ON" : "OFF",
                      _currentState ? HIGH : LOW);
    }

    bool getState(){
        return _currentState;
    }
};
