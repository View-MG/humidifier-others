#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <MPU6050.h>
#include "constant.h"

class GyroService {
private:
    MPU6050 mpu;
    bool _initialized = false;

    float _pitch = 0.0f;
    float _roll  = 0.0f;
    TiltState _state = TILT_NORMAL;

    unsigned long _lastRead = 0;
    const unsigned long READ_INTERVAL_MS = 50; // อ่านทุก ~50ms

public:
    GyroService() {}

    void begin() {
        Wire.begin(MPU_SDA, MPU_SCL);

        Serial.println("[MPU] Init...");
        mpu.initialize();

        if (mpu.testConnection()) {
            Serial.println("[MPU] Connected ✔");
            _initialized = true;
        } else {
            Serial.println("[MPU] Connection FAILED ✖");
            _initialized = false;
        }
    }

    void update() {
        if (!_initialized) return;

        unsigned long now = millis();
        if (now - _lastRead < READ_INTERVAL_MS) return;
        _lastRead = now;

        int16_t ax, ay, az;
        mpu.getAcceleration(&ax, &ay, &az);

        float axf = (float)ax;
        float ayf = (float)ay;
        float azf = (float)az;

        // คำนวณ pitch / roll แบบง่ายจาก accelerometer
        _pitch = atan2f(ayf, sqrtf(axf * axf + azf * azf)) * 180.0f / PI;
        _roll  = atan2f(-axf, azf) * 180.0f / PI;

        float devPitch = fabsf(_pitch - MPU_REF_PITCH);
        float devRoll  = fabsf(_roll  - MPU_REF_ROLL);
        float dev = devPitch > devRoll ? devPitch : devRoll;

        TiltState newState;
        if (dev >= MPU_FALL_THRESHOLD_DEG) {
            newState = TILT_FALL;
        } else if (dev >= MPU_WARN_THRESHOLD_DEG) {
            newState = TILT_WARNING;
        } else {
            newState = TILT_NORMAL;
        }

        if (newState != _state) {
            _state = newState;
            const char* label =
                (_state == TILT_NORMAL)  ? "NORMAL" :
                (_state == TILT_WARNING) ? "WARN"   : "FALL";

            Serial.printf("[MPU] State change -> %s (pitch=%.1f roll=%.1f dev=%.1f)\n",
                          label, _pitch, _roll, dev);
        }
    }

    TiltState getTiltState() const {
        return _state;
    }
};
