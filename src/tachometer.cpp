#include <Arduino.h>
#include "config.h"
#include "tachometer.h"

static volatile uint32_t pulse_count = 0;

static void IRAM_ATTR onTachPulse()
{
    pulse_count++;
}

void Tachometer::begin()
{
    pinMode(PIN_TACH, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_TACH), onTachPulse, FALLING);
    last_sample_ms_ = millis();
}

uint32_t Tachometer::update(uint32_t nowMs)
{
    const uint32_t elapsed = nowMs - last_sample_ms_;
    if (elapsed < FAN_TACH_SAMPLE_MS)
        return rpm_;

    noInterrupts();
    const uint32_t pulses = pulse_count;
    pulse_count = 0;
    interrupts();

    rpm_ = (pulses * 60000UL) / (elapsed * FAN_TACH_PULSES_PER_REV);
    last_sample_ms_ = nowMs;
    return rpm_;
}
