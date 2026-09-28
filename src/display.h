#pragma once

#include <stdint.h>

/* 16x2 I2C LCD (PCF8574 backpack) status display. */
class Display {
public:
    void begin();
    void show(float tempC, uint8_t duty, bool alarm);
};
