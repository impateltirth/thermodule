/*
 * Thermodule — ESP32 thermal management.
 *
 * Loop: read temperature (NTC thermistor) -> compute fan duty with
 * hysteresis -> drive 25 kHz PWM (LEDC) -> update I2C LCD ->
 * serial telemetry.
 */
#include <Arduino.h>
#include "config.h"
#include "temp_sensor.h"
#include "fan.h"
#include "display.h"
#include "tachometer.h"

static TempSensor sensor;
static Fan fan;
static Display display;
static Tachometer tachometer;

void setup()
{
    Serial.begin(115200);
    sensor.begin();
    fan.begin();
    display.begin();
    tachometer.begin();
    Serial.println("Thermodule booted");
}

void loop()
{
    static uint32_t last = 0;
    uint32_t now = millis();

    if (now - last >= CONTROL_PERIOD_MS) {
        last = now;
        float temp = sensor.readCelsius();
        uint8_t duty = fan.update(temp);
        uint32_t rpm = tachometer.update(now);
        display.show(temp, duty, fan.alarm(), fan.sensorFault());
        Serial.printf("T=%.1fC duty=%u rpm=%lu alarm=%d sensor_fault=%d\n",
                      (double)temp, duty, (unsigned long)rpm,
                      fan.alarm(), fan.sensorFault());
    }
}
