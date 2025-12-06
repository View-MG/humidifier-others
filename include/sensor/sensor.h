#pragma once
#include <Arduino.h>
#include <Keypad.h>
#include "constant.h"

class SensorService {
private:
    Keypad _keypad;

public:
    SensorService() 
        : _keypad(makeKeymap(KEY_MAP), (byte*)ROW_PINS, (byte*)COL_PINS, KEYPAD_ROWS, KEYPAD_COLS) 
    {}

    void begin() {
        pinMode(PIN_WATER_LEVEL, INPUT);
        pinMode(PIN_TILT, INPUT_PULLUP);
    }

    int getWaterRaw() { return analogRead(PIN_WATER_LEVEL); }
    
    int getWaterPercent() {
        return constrain(map(getWaterRaw(), 0, 4095, 0, 100), 0, 100);
    }

    bool getTilt() { return false; } // แก้ตาม Hardware

    char getKey() { return _keypad.getKey(); }
};