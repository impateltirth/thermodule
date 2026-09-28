#pragma once

#include <stdint.h>

/* PWM fan driver (ESP32 LEDC) with hysteresis + slew-rate-limited control.
 * update() returns the new duty (0-255) and drives the pin. */
class Fan {
public:
    void begin();
    uint8_t update(float tempC);
    uint8_t duty() const { return duty_; }
    bool alarm() const { return alarm_; }  // true when temp >= TEMP_MAX_C

private:
    uint8_t duty_ = 0;
    bool alarm_ = false;
};
