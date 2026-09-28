#pragma once

/* 10k NTC thermistor on an ADC pin, converted via Steinhart-Hart.
 * See config.h for the assumed divider topology. */
class TempSensor {
public:
    void begin();
    float readCelsius();  // 16-sample averaged, in degrees C
};
