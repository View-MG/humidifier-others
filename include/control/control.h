#pragma once
#include <Arduino.h>
#include "constant.h"

class ControlService {
private:
    int  _pin;
    bool _state;   // true = ON, false = OFF

public:
    ControlService() : _pin(PIN_CONTROL), _state(false) {}

    void begin() {
        pinMode(_pin, OUTPUT);
        digitalWrite(_pin, LOW); // เริ่มต้น OFF

        Serial.printf("[CONTROL] Init PIN=%d -> OFF\n", _pin);
    }

    void setState(bool on) {
        _state = on;

        // ถ้ารีเลย์ Active LOW ให้สลับ HIGH/LOW ตรงนี้
        digitalWrite(_pin, _state ? HIGH : LOW);

        Serial.printf("[CONTROL] Set -> %s (PIN=%d, LEVEL=%d)\n",
                      _state ? "ON" : "OFF",
                      _pin,
                      _state ? HIGH : LOW);
    }

    bool getState() const {
        return _state;
    }
};
