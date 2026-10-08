#include <Arduino.h>
#include "temp_sensor.h"
#include "config.h"

void TempSensor::begin()
{
    pinMode(PIN_THERMISTOR, INPUT);
    analogSetAttenuation(ADC_11db);  // ~0-3.3 V range on ESP32 ADC
}

float TempSensor::readCelsius()
{
    // Average 16 readings to knock down ADC noise.
    uint32_t raw = 0;
    for (uint8_t i = 0; i < 16; i++) {
        raw += analogRead(PIN_THERMISTOR);
        delay(2);
    }
    float adc = raw / 16.0f;
    // Either rail indicates an open/shorted divider or unusable reading.
    // NAN is handled as a fail-safe full-fan condition by Fan::update().
    if (adc < 1.0f || adc > 4094.0f)
        return NAN;

    float v = adc / 4095.0f * ADC_VREF;
    // Divider: 3V3 -- Rs --+-- Rt -- GND, ADC at +.
    // Vmid = 3V3 * Rt / (Rs + Rt)  =>  Rt = Rs * Vmid / (3V3 - Vmid)
    float r_therm = THERM_SERIES_R * v / (ADC_VREF - v);

    // Steinhart-Hart (B-parameter form).
    float steinhart = logf(r_therm / THERM_NOMINAL_R) / THERM_B_COEFF
                    + 1.0f / (THERM_NOMINAL_T + 273.15f);
    return 1.0f / steinhart - 273.15f;
}
